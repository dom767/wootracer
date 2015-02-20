#include "stdafx.h"
#include "Material.h"
#include "Scene.h"
#include "SceneIO.h"
#include "tinyxml.h"
#include "convert.h"
#include "FuncFactory.h"
#include "Functions.h"
#include "Log.h"

DVecFunc* GetFunction(const TiXmlElement* element, const char* function)
{
	return NULL;
/*	const char* functionString = element->Attribute(function);
	
	try
	{
		if (functionString)
		{
			ret = new DVecRoot();
			ret->Read(std::string(functionString));
		}
	}
	catch (int /*e*//*)
	{
		std::stringstream ss;
		ss << "Invalid Function String, " << function << ": " << functionString;
		Log().Print(ss.str().c_str());
	}

	return ret;*/
}


void DMaterial::Cleanup()
{
	delete mDiffuseColour;
	delete mSpecularColour;
	delete mEmissiveColour;
	delete mReflectivity;
	delete mAbsorptionColour;
	mMaterialProgram.Reset();
}

void DMaterial::Read(TiXmlElement* element)
{
	Cleanup();

//	mDiffuseFunction.reset(GetFunction(element, "diffuseFunction"));
//	mSpecularFunction.reset(GetFunction(element, "specularFunction"));
//	mEmissiveFunction.reset(GetFunction(element, "emissiveFunction"));
//	mReflectiveFunction.reset(GetFunction(element, "reflectiveFunction"));
	mDiffuseColour = DCF::CreateFromXML((TiXmlElement*) element->FirstChildElement("DIFFUSECOLOUR"));
	mSpecularColour = DCF::CreateFromXML((TiXmlElement*) element->FirstChildElement("SPECULARCOLOUR"));
	mEmissiveColour = DCF::CreateFromXML((TiXmlElement*) element->FirstChildElement("EMISSIVECOLOUR"));
	Convert::StrToFloat(element->Attribute("specularPower"), mSpecularPower);
	mReflectivity = DCF::CreateFromXML((TiXmlElement*) element->FirstChildElement("REFLECTIVITYCOLOUR"));
	Convert::StrToFloat(element->Attribute("opacity"), m_Opacity);
	Convert::StrToFloat(element->Attribute("density"), m_Density);
	mAbsorptionColour = DCF::CreateFromXML((TiXmlElement*) element->FirstChildElement("ABSORPTIONCOLOUR"));
	Convert::StrToFloat(element->Attribute("shininess"), m_Shininess);
	const char* materialFunction = element->Attribute("materialFunction");
	if (materialFunction)
	{
		mMaterialProgram.Create(std::string(materialFunction));
	}
}

namespace
{
	float CalculateBeersLaw(const float distance, const float Density)
	{
		return expf(-
			Density*distance);
	}
}

DVector3 diortho(DVector3 v)
{
    //  See : http://lolengine.net/blog/2013/09/21/picking-orthogonal-vector-combing-coconuts
    return abs(v[0]) > abs(v[2]) ? DVector3(-v[1], v[0], 0.0)  : DVector3(0.0, -v[2], v[1]);
}
 
DVector3 digetSampleBiased(const DScene* scene, const DRayContext& rayContext, DVector3 dir, float power)
{
	dir.Normalise();
	DVector3 o1 = diortho(dir);
	o1.Normalise();
	DVector3 o2 = dir.Cross(o1);
	o2.Normalise();

	DVector2 randVec = scene->GetRandom2D(rayContext.mPixelIndex, rayContext.mSampleIndex, rayContext.mSubFrame);
	float rx = randVec.x;
	float ry = randVec.y;
	rx = rx * 2.0f * 3.14159265f;
	ry = powf(ry, 1.0f/(power+1.0f));
	float oneminus = sqrtf(1.0f-ry*ry);
	return o1*cosf(rx)*oneminus+o2*sin(rx)*oneminus+dir*ry;
}

void DMaterial::CalculateColour(DColour &out_colour,
	const DVector3& hitPos,
	const int hitId,
	const DVector3& normal,
	const DColour& colour,
	const DRayContext &rRayContext,
	const bool ignoreLighting) const
{
	int recursionRemaining = rRayContext.m_RecursionRemaining;
	const DVector3 eyeVector = rRayContext.m_Ray.GetDirection();

	const DScene* const scene = rRayContext.m_Scene;
	DColour diffuseLight(0.f,0.f,0.f), specularLight(0.f,0.f,0.f);
	DVector3 reflection = eyeVector - normal*(normal.Dot(eyeVector)*2.f);

	// shininess perturb
	DVector3 perturb = scene->GetRandomDirection3d(rRayContext);//DVector3(scene->GetRandom()*2-1, scene->GetRandom()*2-1, scene->GetRandom()*2-1);
	perturb *= 1.0f - m_Shininess;
	reflection += perturb;
	reflection.Normalise();

	DFunctionState funcState;
	funcState.mPosition = hitPos;
	funcState.mDiffuse = mDiffuseColour->GetColour(hitPos);
	funcState.mSpecular = mSpecularColour->GetColour(hitPos);
	funcState.mReflectivity = mReflectivity->GetColour(hitPos);
	funcState.mEmissive = mEmissiveColour->GetColour(hitPos);
	funcState.mAbsorption = mAbsorptionColour->GetColour(hitPos);

	// run shader if present
	if (mMaterialProgram.Compiled())
	{
		mMaterialProgram.Run(funcState);

		// clamp
		funcState.mDiffuse.Clamp(DColour(0,0,0), DColour(1,1,1));
		funcState.mSpecular.Clamp(DColour(0,0,0), DColour(1,1,1));
		funcState.mReflectivity.Clamp(DColour(0,0,0), DColour(1,1,1));
		funcState.mEmissive.Clamp(DColour(0,0,0), DColour(100000,100000,100000));
		funcState.mAbsorption.Clamp(DColour(0,0,0), DColour(1,1,1));
	}

	// return diffuse and specular lighting contributions
	DRayContext colourContext(rRayContext);
	colourContext.m_Ray = DRay(hitPos, normal);

	if (scene && !ignoreLighting)
		scene->CalculateLighting(colourContext,
			hitId,
			reflection,
			mSpecularPower,
			funcState.mSpecular,
			diffuseLight,
			specularLight,
			recursionRemaining);

	// multiply the diffuse colour by the surface colour
	diffuseLight *= /*(mDiffuseFunction.get() ? DColour(mDiffuseFunction->Evaluate(hitPos)) : */funcState.mDiffuse/*)*/ * colour;

	// calculate the colour of the surface
	out_colour = diffuseLight + specularLight + funcState.mEmissive;

	// calculate reflection
	DCollisionResponse response;
	response.mIgnoreObjectId = 0;

	// calculate diffuse interreflection
	if (scene->IsPathTracer())
	{		
		DVector3 randomInter = digetSampleBiased(scene, rRayContext, normal, 1);

		DRayContext pathTrace(rRayContext);
		pathTrace.m_Ray = DRay(hitPos, randomInter);
		pathTrace.m_RecursionRemaining = recursionRemaining==0 ? -1 : (recursionRemaining==1 ? 0 : 1);
		pathTrace.m_RequestFlags |= RequestLighting;
		scene->Intersect(pathTrace, response);
			
		DColour addition = response.mColour;
		addition *= funcState.mDiffuse;
		out_colour += addition;
	}

	DColour reflectivityCol = funcState.mReflectivity;
	if (scene && reflectivityCol.Max()>0)
	{
		DRayContext reflectionContext(rRayContext);
		reflectionContext.m_Ray = DRay(hitPos, reflection);
		reflectionContext.m_RecursionRemaining = recursionRemaining-1;

		scene->Intersect(reflectionContext, response);
		out_colour.mRed += response.mColour.mRed*reflectivityCol.mRed;
		out_colour.mGreen += response.mColour.mGreen*reflectivityCol.mGreen;
		out_colour.mBlue += response.mColour.mBlue*reflectivityCol.mBlue;
	}

	if (m_Opacity!=1)
	{/*
		const double n = rRayContext.m_RefractiveIndex / m_RefractiveIndex;
		const double cosI = normal.Dot(eyeVector);
		double sinT2 = n*n*(1.0 - cosI*cosI);
		if (sinT2 > 1)
		{
			sinT2 = 1;
		}
		DVector3 refractionVector = (eyeVector * n) - (normal * (n + sqrtf(1 - sinT2)));
		refractionVector.Normalise();
*/
		const float n = rRayContext.m_RefractiveIndex / m_RefractiveIndex;
		const float cos1 = -normal.Dot(eyeVector);
		const float cos2 = sqrtf(1 - ((n * n) * (1 - (cos1 * cos1))));
		DVector3 refractionVector;
		if (cos1<0)
			refractionVector = (eyeVector * n) + (normal * ((n*cos1) + cos2));
		else
			refractionVector = (eyeVector * n) + (normal * ((n*cos1) - cos2));
		refractionVector.Normalise();

		// calculate passthrough
		DCollisionResponse RefractionResponse;
		DRayContext refractionContext(rRayContext);
		refractionContext.m_Ray = DRay(hitPos, refractionVector);
		refractionContext.m_RecursionRemaining--;
		refractionContext.m_RefractiveIndex = m_RefractiveIndex;
		
//		RefractionResponse.mIgnoreObjectId = hitId;
		RefractionResponse.m_WithinObjectId = hitId;
		if (scene)
			scene->Intersect(refractionContext, RefractionResponse);

		out_colour = (out_colour*m_Opacity) + (RefractionResponse.mColour*(1-m_Opacity));
	}
}