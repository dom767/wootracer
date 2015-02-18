#pragma once

#include "RenderObject.h"

class DRenderCube : public DRenderObject
{
//	DECLARE_SCENE_OBJECT()

public:
	DRenderCube()
	{
		mCentre = DVector3(0,0,0);
		mNormal = DVector3(0,1,0);
		mSideLength = 1;
		mAABoundingBox = DAABoundingBox(mCentre, mScale[0], mScale[1], mScale[2]);
		mAABoundingBox.TransformBy(mRotation);
	}
/*	DRenderCube(DVector3& Centre, float sideLength, DMaterial& Material)
		: DRenderObject(Centre, Material, DAABoundingBox(Centre, sideLength, sideLength, sideLength)),
		mSideLength(sideLength)
	{
	}*/
	DRenderObject* InternalClone()
	{
		DRenderCube* ret = new DRenderCube();
		ret->mSideLength = mSideLength;
		ret->mNormal = mNormal;
		return ret;
	}
	virtual void PostLoad()
	{
		mAABoundingBox = DAABoundingBox(mCentre, mScale[0], mScale[1], mScale[2]);
		mAABoundingBox.TransformBy(mRotation);
	}
	void Read(TiXmlElement* element);

	float mSideLength;
	DVector3 mNormal;

protected:
	virtual bool InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const;
	virtual void InternalRead(TiXmlElement* element);

};
