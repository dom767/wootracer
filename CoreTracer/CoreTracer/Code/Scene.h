#pragma once
#include "KDTree.h"
#include "Colour.h"
#include "Background.h"
#include "Random.h"

class DRenderSphere;
class DRenderCircle;
class DRenderSquare;
class TiXmlElement;
class DLight;

struct DCollisionResponse
{
	DCollisionResponse() : mIgnoreObjectId(-1) {}
	float mDistance;
	DColour mColour;
	int mIgnoreObjectId;
	int m_WithinObjectId;
	DVector3 mHitPosition;
	DVector3 mNormal;

};

enum ERequestType
{
	RequestColour=0x1,
	RequestDistance=0x2,
	RequestBackface=0x4,
	RequestLighting=0x8,
	RequestNoSpecular=0x01,
};

struct DRayContext
{
	DRayContext(const DScene* scene, DRay ray, int requestType, int maxRecursion, const float refractionIndex, int pixelIndex) :
m_Scene(scene), m_RecursionRemaining(maxRecursion), m_RefractiveIndex(refractionIndex), m_RequestFlags(requestType), m_Ray(ray), mPixelIndex(pixelIndex), mSampleIndex(0) {}

	const static float AirRefractionIndex;
	DRay m_Ray;
	int m_RequestFlags;
	float m_RefractiveIndex;
	int m_RecursionRemaining;
	const DScene* m_Scene;
	int mSampleIndex;
	int mPixelIndex;
};

class DScene
{
public:
	DScene();
	~DScene();

	void operator=(const DScene& scene);
	bool Intersect(DRayContext& RayContext, DCollisionResponse& out_Response, bool debuginfo=false) const;
	void Read(TiXmlElement* element);
	void CreateRenderObject(TiXmlElement* objectXml);
	void Render(int width, int height, DColour *renderBuffer);
	void PrepareRender();
	void RenderRow(int width, int height, int row, DColour *renderBuffer);
	void CalculateLighting(DRayContext &rayContext, int hitId, const DVector3& reflection, float specularPower,
		const DColour& specularColour, DColour& out_diffuseLight, DColour& out_specularLight, int recursionRemaining) const;
	DColour GetFogColour(const DRay &ray);
	void Reset();
	void RegisterSceneObjects();
	int GetMaximumRecursion() const {return mMaximumRecursion;}
	void SetRandomSeed(int seed) const;
	float GetRandom() const {return float(mRandom.GetRandom()&0xFFFF) / float(0xFFFF);}
	bool IsPathTracer() const {return mPathTracer>0;}

	mutable __int64 mRayCount;
	DKDTree mKDTree;
	std::vector<DRenderObject*> mRenderObjects;
	std::vector<DLight*> mLights;

private:
	int mMaximumRecursion;
	int mCurrentId;
	int mCanvasWidth;
	int mCanvasHeight;
	DBackground mBackground;
	DRandom mRandom;
	int mPathTracer;
};