#pragma once
#include "KDTree.h"
#include "Colour.h"
#include "Background.h"
#include "Random.h"
#include "Vector2.h"
#include "RandomSequence.h"

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
	RequestZeroLighting=0x10,
	RequestNoSpecular=0x01,
};

struct DRayContext
{
	DRayContext(const DScene* scene, DRay ray, int requestType, int maxRecursion, const float refractionIndex, int pixelIndex, int subFrame) :
m_Scene(scene), m_RecursionRemaining(maxRecursion), m_RefractiveIndex(refractionIndex), m_RequestFlags(requestType), m_Ray(ray), mPixelIndex(pixelIndex), mSampleIndex(4), mSubFrame(subFrame) {}

	const static float AirRefractionIndex;
	DRay m_Ray;
	int m_RequestFlags;
	float m_RefractiveIndex;
	int m_RecursionRemaining;
	const DScene* m_Scene;
	mutable int mSampleIndex;
	int mPixelIndex;
	int mSubFrame;
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
	DVector2 GetRandom2D(unsigned int pixelIndex, unsigned int sampleIndex, unsigned int subFrame) const;
	DVector2 GetRandom2D(const DRayContext& ray) const;
	DVector3 GetRandomDirection3d(const DRayContext &rayContext) const;
	bool IsPathTracer() const {return mPathTracer>0;}
	bool IsCaustics() const {return mCaustics;}

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
	DRandomSequence mRandomSequence;
	int mPathTracer;
	bool mCaustics;
};