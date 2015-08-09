#include "stdafx.h"
#include "Scene.h"
#include "RenderSphere.h"
#include "RenderCircle.h"
#include "RenderSquare.h"
#include "RenderCube.h"
#include "RenderManger.h"
#include "RenderCylinder.h"
#include "RenderDistance.h"
#include "RenderSVO.h"
#include "RenderFractal.h"
#include "ColourFunction.h"
#include <stdlib.h>
#include "tinyxml.h"
#include "convert.h"
#include "Light.h"
#include "Log.h"

const float DRayContext::AirRefractionIndex = 1.003f;
const int numRandomSequences = 1024;
	
DScene::DScene() : mMaximumRecursion(2), mCanvasWidth(400), mCanvasHeight(300), mRandomSequence(DRandomSequence::RelaxingPoisson, numRandomSequences, 753)
{
	mPathTracer = false;
	mCaustics = false;
	mShadows = true;
	mRayCount=0;
	mRandomSequence.GenerateSequences();
}

DScene::~DScene()
{
	Cleanup();
}

void DScene::Cleanup()
{
	for (unsigned int i=0; i<mRenderObjects.size(); ++i)
	{
		delete mRenderObjects[i];
	}

	for (unsigned int i=0; i<mLights.size(); ++i)
	{
		delete mLights[i];
	}
}

void DScene::operator=(const DScene& scene)
{
//	std::map<DRenderObject*, DRenderObject*> ObjectMap;

//	for (unsigned int i=0; i<scene.mRenderObjects.size(); i++)
	//{
		//DRenderObject* newObject = scene.mRenderObjects[i]->Clone();
	//}
//	mKDTree = scene.mKDTree.Clone(ObjectMap);
/*	mLights = scene.mLights;
	mFieldOfView = scene.mFieldOfView;
	mCanvasHeight = scene.mCanvasHeight;
	mCanvasWidth = scene.mCanvasWidth;
	mRenderBuffer = scene.mRenderBuffer;
	mMaximumRecursion = scene.mMaximumRecursion;
	mGlobalLighting = scene.mGlobalLighting;
	mKDTree = scene.mKDTree;*/
}

void DScene::Read(TiXmlElement* element)
{
	Cleanup();

    Convert::StrToInt(element->Attribute("pathTracer"), mPathTracer);
	
	Convert::StrToBool(element->Attribute("caustics"), mCaustics);

	Convert::StrToBool(element->Attribute("shadows"), mShadows);

	Convert::StrToInt(element->Attribute("recursions"), mMaximumRecursion);

	TiXmlElement* backgroundXml = (TiXmlElement*) element->FirstChildElement("BACKGROUND");
	if (backgroundXml)
	{
		mBackground.Read(backgroundXml);
	}

	TiXmlElement* lightXml = (TiXmlElement*) element->FirstChildElement("LIGHT");
	while (lightXml)
	{
		mLights.push_back(DLight::CreateFromXML(lightXml));
		lightXml = (TiXmlElement*) lightXml->NextSiblingElement("LIGHT");
	}

	TiXmlElement* objectXml = (TiXmlElement*) element->FirstChildElement("OBJECT");
	while (objectXml)
	{
		CreateRenderObject(objectXml);
		objectXml = (TiXmlElement*) objectXml->NextSiblingElement("OBJECT");
	}
}

void DScene::CreateRenderObject(TiXmlElement* objectXml)
{
	DRenderObject* object = NULL;
	int type;
	
	Convert::StrToInt(objectXml->Attribute("type"), type);
	
	switch (type) 
	{
	case 0:
		object = new DRenderSphere();
		object->Read(objectXml);
		break;
	case 1:
		object = new DRenderCircle();
		object->Read(objectXml);
		break;
	case 2:
		object = new DRenderCube();
		object->Read(objectXml);
		break;
	case 3:
		object = new DRenderSquare();
		object->Read(objectXml);
		break;
	case 4:
		object = new DRenderManger();
		object->Read(objectXml);
		break;
	case 5:
		object = new DRenderCylinder();
		object->Read(objectXml);
		break;
	case 6:
		object = new DRenderDistance();
		object->Read(objectXml);
		break;
	case 7:
		object = new DRenderSVO();
		object->Read(objectXml);
		break;
	case 8:
		object = new DRenderFractal();
		object->Read(objectXml);
		break;
	};

	if (object)
	{
		object->SetObjectId(mRenderObjects.size()+1);
		mRenderObjects.push_back(object);
	}
}

DRenderObject* DScene::GetObject(int idx) const
{
	if (idx<1) return NULL;

	return mRenderObjects[idx-1];
}

void DScene::Render(int width, int height, DColour *renderBuffer)
{
//	GetRenderTimer().StartTimer();

	// set up the KD Tree
	PrepareRender();
//	mCamera.Render(*this, width, height, renderBuffer);
}

void DScene::PrepareRender()
{
	mKDTree.Initialise(mRenderObjects);
}

void DScene::RenderRow(int width, int height, int row, DColour *renderBuffer)
{
	//mCamera.RenderRow(*this, width, height, row, renderBuffer);
}

bool DScene::Intersect(DRayContext& RayContext, DCollisionResponse& out_Response, bool debuginfo) const
{
	mRayCount++;
	if (RayContext.m_RecursionRemaining<0)
	{
		LOG(Info, "Recursion limit reached, bailout");
		return false;
	}

	bool hit = true;
	int hitId = mKDTree.Intersect(RayContext, out_Response, debuginfo);
	if (hitId == 0)
	{
		if ((RayContext.m_RequestFlags&RequestColour)>0 && (RayContext.m_RequestFlags&RequestZeroLighting)==0)
		{
			mBackground.GetSceneryColour(RayContext.m_Ray, out_Response.mColour);
		}

		hit = false;
	}

	// within an object, so factor in the colour
	int numWithin = RayContext.mWithinIdx.size();
	if (numWithin>0)
	{
		float distance = out_Response.mDistance;
		std::vector<int> within;
		for (int i=0; i<numWithin; i++)
		{
			DRenderObject* object = RayContext.m_Scene->GetObject(RayContext.mWithinIdx[i]);
			int oldFlags = RayContext.m_RequestFlags;
			RayContext.m_RequestFlags = RequestBackface|RequestDistance;
			DCollisionResponse within_Response;
			LOG(Info, "Checking for intersection with containing object");
			object->Intersect(RayContext, within_Response);
			if (Log().mErrorLevel==Info)
			{
				std::string kdDebug;
				std::stringstream ss;
				ss<<"Containing Object intersection result : "<<object->GetObjectId();
				ss<<"\r\nRay start : "<<RayContext.m_Ray.GetStart()[0]<<", "<<RayContext.m_Ray.GetStart()[1]<<", "<<RayContext.m_Ray.GetStart()[2];
				ss<<"\r\nRay direction : "<<RayContext.m_Ray.GetDirection()[0]<<", "<<RayContext.m_Ray.GetDirection()[1]<<", "<<RayContext.m_Ray.GetDirection()[2];
				ss<<"\r\nRay recursionRemaining : "<<RayContext.m_RecursionRemaining;
				ss<<"\r\nRay AirRefractionIndex : "<<RayContext.AirRefractionIndex;
				ss<<"\r\nRay RefractiveIndex : "<<RayContext.m_RefractiveIndex;
				ss<<"\r\nRay m_RequestFlags : "<<RayContext.m_RequestFlags;
				ss<<"\r\nCollision point : "<<within_Response.mDistance;
				ss<<"\r\n";
				kdDebug = ss.str();
				LOG(Info, kdDebug.c_str());
			}

			if (i==0)
			{
				if (hitId==0 || within_Response.mDistance<out_Response.mDistance)
				{
					distance = within_Response.mDistance;
					RayContext.m_RequestFlags = oldFlags|RequestBackface|RequestDistance;
//					RayContext.mWithinIdx.erase(RayContext.mWithinIdx.begin()); 
					object->CalculateColour(RayContext, within_Response);
					out_Response = within_Response;
				}
				else
				{
					RayContext.m_RequestFlags = oldFlags;
					if (hitId>0)
						RayContext.m_Scene->GetObject(hitId)->CalculateColour(RayContext, out_Response);
				}

				// absorb
				float density = 1 - exp(-distance * object->GetDensity());
				out_Response.mColour = object->GetAbsorptionColour(RayContext.m_Ray.GetStart()) * density + out_Response.mColour * (1-density);
				// tint
				density = 1 - exp(-distance * object->GetTintDensity());
				out_Response.mColour = out_Response.mColour * ((object->GetDiffuseColour(RayContext.m_Ray.GetStart()) * density) + (DColour(1,1,1) * (1-density)));
			}

			RayContext.m_RequestFlags = oldFlags;

			if (within_Response.mDistance > distance)
			{
				within.push_back(RayContext.mWithinIdx[i]);
			}
		}
		RayContext.mWithinIdx = within;
	}
	else
	{
		if (hitId>0)
			RayContext.m_Scene->GetObject(hitId)->CalculateColour(RayContext, out_Response);
	}

	/*
	DVector3 hitPos;
	
	if (hit)
		hitPos = out_Response.mHitPosition;
	else
		hitPos = RayContext.m_Ray.GetDirection()*1000 + RayContext.m_Ray.GetStart();
			// fog
	DVector3 direction = hitPos - RayContext.m_Ray.GetStart();
	float distance = direction.Magnitude();
	direction.Normalise();
	float heightstart = RayContext.m_Ray.GetStart().mComponent[1];
	float heightend = hitPos.mComponent[1];
	float b = 0.15f;
	float c = 0.08f;
//	float thickness = ((exp(-heightstart)) + (exp(-heightend))) * distance * 0.1;
	float thickness = c * exp(-heightstart * b) * (1 - exp(-distance * direction.mComponent[1] * b)) / (b*direction.mComponent[1]);
//	if (thickness>1) thickness = 1;
	float density = thickness;//1 - exp(-thickness);

//	scene->GetFogColour(rRayContext.m_Ray);

	out_Response.mColour = (DColour(0.6,0.5,0.4) * density) + (out_Response.mColour * (1-density));
	*/
	return hit;
}

void DScene::CalculateLighting(DRayContext &rayContext, int hitId, const DVector3& reflection, float specularPower, const DColour& specularColour, DColour& out_diffuseLight, DColour& out_specularLight) const
{
	std::vector<DLight*>::const_iterator iter = mLights.begin(), iterEnd = mLights.end();

	while (iter!=iterEnd)
	{
		DColour diffuse, specular;
		if ((*iter)->GetLighting(*this, rayContext, reflection, specularPower, specularColour, diffuse, specular))
		{
			out_diffuseLight += diffuse;
			out_specularLight += specular;
		}

		iter++;
	}
}

DColour DScene::GetFogColour(const DRay &ray)
{
	std::vector<DLight*>::const_iterator iter = mLights.begin(), iterEnd = mLights.end();

	while (iter!=iterEnd)
	{
		// for directional - light colour *pow(dot(dir, light),10)

		// for position - light colour * pow(dot(dir, light), 10)

		// for ambient - diffuse += ambient

		// for worldlight - diffuse += ambient

		// for spherelight - light colour * power(dot(dir, light), 10)
	}

	return DColour(0,0,0);
}

void DScene::Reset()
{
	std::vector<DRenderObject*>::const_iterator iter = mRenderObjects.begin(),
		iterEnd = mRenderObjects.end();
	while (iter!=iterEnd)
	{
		delete *iter;
		iter++;
	}
	mRenderObjects.clear();
	mLights.clear();
}

void DScene::SetRandomSeed(int seed) const
{
	mRandom.SetSeed(seed);
}


void DScene::RegisterSceneObjects()
{
//	REGISTER_SCENE_OBJECT(DLight);
//	REGISTER_SCENE_OBJECT(DRenderSphere);
//	REGISTER_SCENE_OBJECT(DRenderCircle);
//	REGISTER_SCENE_OBJECT(DGlobalLighting);
}

DVector2 DScene::GetRandom2D(const DRayContext& ray) const
{
	DVector2 ret = GetRandom2D(ray.mPixelIndex, ray.mSampleIndex, ray.mSubFrame);
	ray.mSampleIndex += 2;
	return ret;
}

DVector2 DScene::GetRandom2D(unsigned int pixelIndex, unsigned int sampleIndex, unsigned int subFrame) const
{
//	return DVector2(mRandom.GetNextFloat(), mRandom.GetNextFloat());
	unsigned int clamppixel = pixelIndex%3592;
	unsigned int sequence = unsigned int(subFrame/256)*4801 + pixelIndex*pixelIndex + sampleIndex;
	
	sequence = ((sequence >> 16) ^ sequence) * 0x45d9f3b;
    sequence = ((sequence >> 16) ^ sequence) * 0x45d9f3b;
    sequence = ((sequence >> 16) ^ sequence);
	
//	sequence = (sequence*2654435761)%(2^32);

	return mRandomSequence.GetValue(sequence%numRandomSequences, subFrame%256);
}

DVector3 DScene::GetRandomDirection3d(const DRayContext &rayContext) const
{
	DVector2 random2d = GetRandom2D(rayContext);
	float azimuth = random2d.x * 2 * 3.14159265f;
	DVector2 dir2d = DVector2(cosf(azimuth), sinf(azimuth));
	float z = (2*random2d.y) - 1;
	DVector2 planar = dir2d * sqrt(1-z*z);
	return DVector3(planar.x, planar.y, z);
}