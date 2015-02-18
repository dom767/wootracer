#pragma once
#include "Vector3.h"
#include "Ray.h"
#include "Material.h"
#include "AABoundingBox.h"
#include "Matrix4.h"

struct DCollisionResponse;
class TiXmlElement;

class DRenderObject : public DSceneObject
{
public:
	DRenderObject() : mScale(1.f, 1.f, 1.f) {mRotation.MakeIdentity();}
	/*DRenderObject(const DVector3& Centre, const DMaterial& Material, const DAABoundingBox& BoundingBox)
		: mScale(1.f,1.f,1.f),
		mCentre(Centre),
		mMaterial(Material),
		mAABoundingBox(BoundingBox),
		mIgnoreWhileLighting(false)
	{
		mRotation.MakeIdentity();
		mTransform.MakeIdentity();
		mTransform.mVal[0][0] = mCentre[0];
		mTransform.mVal[0][1] = mCentre[1];
		mTransform.mVal[0][2] = mCentre[2];
		mTransform.mVal[1][0] = mScale[0];
		mTransform.mVal[1][1] = mScale[1];
		mTransform.mVal[1][2] = mScale[2];
		mTransform.mVal[2][0] = 0;
		mTransform.mVal[2][1] = 0;
		mTransform.mVal[2][2] = 0;
	}*/

	void Read(TiXmlElement* element);

	bool Intersect(const DRayContext& rRayContext, DCollisionResponse& out_Response);
	const DVector3& GetCentre() {return mCentre;}
	const DAABoundingBox& GetAABoundingBox() {return mAABoundingBox;}
	void SetIgnoreWhileLighting(bool IgnoreWhileLighting) {mIgnoreWhileLighting=IgnoreWhileLighting;}
	bool GetIgnoreWhileLighting() const {return mIgnoreWhileLighting;} 
	DRenderObject* Clone();

protected:
	virtual bool InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const=0;
	virtual void InternalRead(TiXmlElement* element)=0;
	virtual DRenderObject* InternalClone() = 0;

	DVector3 mCentre;
	DVector3 mScale;
	DMatrix4 mRotation;
	DMatrix4 mInvRotation;
	DMatrix4 mScaleMat;
	DMatrix4 mInvScaleMat;
	DMatrix4 mTransform;
	
	// cached concatenated matrices
	DMatrix4 mCentreTrans;
	DMatrix4 mDirectionTrans;
	DMatrix4 mInvCentreTrans;
	DMatrix4 mInvDirectionTrans;
	
	DMaterial mMaterial;
	DAABoundingBox mAABoundingBox;
	bool mIgnoreWhileLighting;

};
