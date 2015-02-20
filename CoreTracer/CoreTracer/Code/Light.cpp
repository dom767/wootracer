#include "stdafx.h"
#include "SceneIO.h"
#include "Light.h"
#include "tinyxml.h"
#include "convert.h"
#include "Ray.h"
#include "Scene.h"

DLight* DLight::CreateFromXML(TiXmlElement* element)
{
	DLight* light;
	int type = 0;
	Convert::StrToInt(element->Attribute("type"), type);
	
	switch (type) 
	{
	case 0:
		light = new DPointLight();
		break;
	case 1:
		light = new DWorldLight();
		break;
	case 2:
		light = new DAmbientLight();
		break;
	case 3:
		light = new DDirectionalLight();
		break;
	case 4:
		light = new DSphereLight();
		break;
	};

	light->Read(element);
	return light;
}

void DAmbientLight::Read(TiXmlElement* element)
{
	Convert::StrToCol(element->Attribute("colour"), mColour);
}

bool DAmbientLight::GetLighting(const DScene& scene, const DRayContext &rayContext, const DVector3& reflection, const float specularPower, const DColour& specularColour, DColour& out_Diffuse, DColour& out_Specular, int recursionRemaining)
{
	out_Diffuse = mColour;
	out_Specular = DColour(0,0,0);
	return true;
}

DPointLight::DPointLight() : mPosition(0,0,0), mColour(0,0,0)
{
}

void DPointLight::Read(TiXmlElement* element)
{
	Convert::StrToVec3(element->Attribute("position"), mPosition);
	Convert::StrToCol(element->Attribute("colour"), mColour);
}

bool DPointLight::GetLighting(const DScene& scene, const DRayContext &rayContext, const DVector3& reflection, const float specularPower, const DColour& specularColour, DColour& out_Diffuse, DColour& out_Specular, int recursionRemaining)
{
	const DRay& ray = rayContext.m_Ray;

	DRay lightHit;
	DVector3 direction = mPosition - ray.GetStart();
	
	// cache this for attenuation
	float lightDistanceSquared = direction.MagnitudeSquared();
	
	// build ray for shadow detection
	direction.Normalise();
	lightHit.SetDirection(direction);
	lightHit.SetStart(ray.GetStart());

	DRayContext lightContext(rayContext);
	lightContext.m_RequestFlags = RequestLighting;
	lightContext.m_Ray = lightHit;

	DCollisionResponse response;
	response.mIgnoreObjectId = 0;
	bool hit = scene.Intersect(lightContext, response);
	if (!hit || ((response.mDistance*response.mDistance)>lightDistanceSquared))
	{
		float diffuse = lightHit.GetDirection().Dot(ray.GetDirection());
		float specular = reflection.Dot(lightHit.GetDirection());
		if (specular<0) specular = 0;
		specular = pow(specular, specularPower);
		const float edgeFade = 0.2f;
		if (diffuse < -edgeFade) specular = 0;
		else if (diffuse < 0.f)
		{
			specular *= (diffuse+edgeFade)/edgeFade;
		}

		if (diffuse<0) diffuse=0;
		diffuse *= diffuse;

		float attenuation = 1 / lightDistanceSquared;
		out_Diffuse = DColour(diffuse, diffuse, diffuse)*(mColour)*attenuation;
		out_Specular = DColour(specular, specular, specular)*specularColour*(mColour)*attenuation;
		return true;
	}
	return false;
}

void DWorldLight::Read(TiXmlElement* element)
{
	Convert::StrToCol(element->Attribute("colour"), mColour);
	Convert::StrToInt(element->Attribute("samples"), mSamples);
}

DVector3 ortho(DVector3 v) {
    //  See : http://lolengine.net/blog/2013/09/21/picking-orthogonal-vector-combing-coconuts
    return abs(v[0]) > abs(v[2]) ? DVector3(-v[1], v[0], 0.0)  : DVector3(0.0, -v[2], v[1]);
}
 
DVector3 getSampleBiased(const DRayContext &rayContext, const DScene& scene, DVector3 dir, float power)
{
	dir.Normalise();
	DVector3 o1 = ortho(dir);
	o1.Normalise();
	DVector3 o2 = dir.Cross(o1);
	o2.Normalise();

	DVector2 randVec = scene.GetRandom2D(rayContext.mPixelIndex, rayContext.mSampleIndex, rayContext.mSubFrame);
	float rx = randVec.x;
	float ry = randVec.y;
	rx = rx * 2.0f * 3.14159265f;
	ry = powf(ry, 1.0f/(power+1.0f));
	float oneminus = sqrtf(1.0f-ry*ry);
	return o1*cosf(rx)*oneminus+o2*sin(rx)*oneminus+dir*ry;
}

bool DWorldLight::GetLighting(const DScene& scene, const DRayContext &rayContext, const DVector3& reflection, const float specularPower, const DColour& specularColour, DColour& out_Diffuse, DColour& out_Specular, int recursionRemaining)
{
	const DRay& ray = rayContext.m_Ray;
	bool ret = false;
	int i=0, hits=0;
	int sampleNum=0;

	while (i<mSamples)
	{
		i++;
		
		DRay lightHit;
		float diffuse;
		DVector3 direction = getSampleBiased(rayContext, scene, ray.GetDirection(), 1);
	
		// build ray for shadow detection
		lightHit.SetDirection(direction);
		lightHit.SetStart(ray.GetStart());

		DRayContext lightContext(rayContext);
		lightContext.m_RequestFlags = RequestLighting;
		lightContext.m_Ray = lightHit;

		diffuse = lightHit.GetDirection().Dot(ray.GetDirection());

		DCollisionResponse response;
		response.mIgnoreObjectId = 0;
		if (!scene.Intersect(lightContext, response))
		{
			out_Diffuse += mColour;
			ret = true;
		}
	}
	
	out_Diffuse /= float(mSamples);
	return ret;
}

void DDirectionalLight::Read(TiXmlElement* element)
{
	Convert::StrToVec3(element->Attribute("direction"), mDirection);
	mDirection.Normalise();
	Convert::StrToCol(element->Attribute("colour"), mColour);
	Convert::StrToFloat(element->Attribute("area"), mArea);
}

bool DDirectionalLight::GetLighting(const DScene& scene, const DRayContext &rayContext, const DVector3& reflection, const float specularPower, const DColour& specularColour, DColour& out_Diffuse, DColour& out_Specular, int recursionRemaining)
{
	const DRay& ray = rayContext.m_Ray;
	DRay lightHit;
	DVector3 direction = mDirection;
	DVector3 jitter = getSampleBiased(rayContext, scene, direction, 0);
	jitter *= mArea;
	direction += jitter;
	direction.Normalise();
	
	// build ray for shadow detection
	lightHit.SetDirection(direction);
	lightHit.SetStart(ray.GetStart());

	DRayContext lightContext(rayContext);
	lightContext.m_RequestFlags = RequestLighting;
	lightContext.m_Ray = lightHit;

	DCollisionResponse response;
	response.mIgnoreObjectId = 0;
	bool hit = scene.Intersect(lightContext, response);
	if (!hit)
	{
		float diffuse = lightHit.GetDirection().Dot(ray.GetDirection());
		float specular = reflection.Dot(lightHit.GetDirection());
		if (specular<0) specular = 0;
		specular = pow(specular, specularPower);
		const float edgeFade = 0.2f;
		if (diffuse < -edgeFade) specular = 0;
		else if (diffuse < 0.f)
		{
			specular *= (diffuse+edgeFade)/edgeFade;
		}

		if (diffuse<0) diffuse=0;
		diffuse *= diffuse;

		out_Diffuse = DColour(diffuse, diffuse, diffuse)*(mColour);
		out_Specular = DColour(specular, specular, specular)*specularColour*(mColour);
		return true;
	}
	return false;
}

void DSphereLight::Read(TiXmlElement* element)
{
	Convert::StrToVec3(element->Attribute("position"), mPosition);
	Convert::StrToCol(element->Attribute("colour"), mColour);
	Convert::StrToFloat(element->Attribute("radius"), mRadius);
	Convert::StrToInt(element->Attribute("samples"), mSamples);
}

bool DSphereLight::GetLighting(const DScene& scene, const DRayContext &rayContext, const DVector3& reflection, const float specularPower, const DColour& specularColour, DColour& out_Diffuse, DColour& out_Specular, int recursionRemaining)
{
	const DRay& ray = rayContext.m_Ray;
	DRay lightHit;
	DVector3 direction;
	int totalHits = 0;
	
	out_Diffuse = DColour(0,0,0);
	out_Specular = DColour(0,0,0);

	int totalSamples=0;

	for (int i=0; (i<mSamples) && (totalSamples<100+(mSamples*mSamples)); i++)
	{
		// get random position in sphere
		DVector3 randVec = scene.GetRandomDirection3d(rayContext);

		DVector3 lightPos = randVec;
		lightPos *= mRadius;
		lightPos += mPosition;
		direction = lightPos - ray.GetStart();
		
		// cache this for attenuation
		float lightDistanceSquared = direction.MagnitudeSquared();
		direction.Normalise();

		//if (direction.Dot(randVec)<0)
		{
			
			// build ray for shadow detection
			//direction.Normalise();
			lightHit.SetDirection(direction);
			lightHit.SetStart(ray.GetStart());

			DRayContext lightContext(rayContext);
			lightContext.m_RequestFlags = RequestLighting;
			lightContext.m_Ray = lightHit;

			float diffuse = lightHit.GetDirection().Dot(ray.GetDirection());

			DCollisionResponse response;
			response.mIgnoreObjectId = 0;
			bool hit = scene.Intersect(lightContext, response);
			if (!hit || ((response.mDistance*response.mDistance)>lightDistanceSquared))
			{
				float diffuse = lightHit.GetDirection().Dot(ray.GetDirection());

				float specular = reflection.Dot(lightHit.GetDirection());
				if (specular<0) specular = 0;
				specular = pow(specular, specularPower);
				const float edgeFade = 0.2f;
				if (diffuse < -edgeFade) specular = 0;
				else if (diffuse < 0.f)
				{
					specular *= (diffuse+edgeFade)/edgeFade;
				}

				if (diffuse<0) diffuse=0;
				diffuse *= diffuse;

				float attenuation = (1+(sqrt(lightDistanceSquared)/mRadius));
				attenuation = attenuation * attenuation;
				attenuation = 1 / attenuation;
				out_Diffuse += DColour(diffuse, diffuse, diffuse)*(mColour)*attenuation;
				out_Specular += DColour(specular, specular, specular)*specularColour*(mColour)*attenuation;

				totalHits++;
			}
		}
//		else
	//	{
		//	i--;
	//	}
		totalSamples++;
	}

	out_Diffuse /= (float)totalHits;
	out_Specular /= (float)totalHits;

	if (totalHits>0)
		return true;
	else
		return false;
}
