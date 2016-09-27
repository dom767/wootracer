#pragma once

#include "RenderObject.h"

class DRenderCircle : public DRenderObject
{
public:
	DRenderCircle()
	{
		mCentre = DVector3(0,0,0);
		mNormal = DVector3(0,1,0);
		mSize = 1;
		mAABoundingBox = DAABoundingBox(mCentre, mScale[0]*mSize, 0, mScale[2]*mSize);
		mAABoundingBox.TransformBy(mRotation);
	}

/*	DRenderCircle(DVector3& Centre, DVector3& Normal, float Size, DMaterial& Material)
		: DRenderObject(Centre, Material, DAABoundingBox(Centre, Size, 0, Size)),
		mNormal(Normal), mSize(Size)
	{
		mAABoundingBox = DAABoundingBox(mCentre, aascale*mSize, 0, aascale*mSize);
	}
	*/
	virtual void PostLoad()
	{
		mAABoundingBox = DAABoundingBox(mCentre, mScale[0]*mSize, 0, mScale[2]*mSize);
		mAABoundingBox.TransformBy(mRotation);
	}

	DRenderObject* InternalClone()
	{
		DRenderCircle* ret = new DRenderCircle();
		ret->mSize = mSize;
		ret->mNormal = mNormal;
		return ret;
	}

	float mSize;
	DVector3 mNormal;

protected:
	virtual bool InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const;
	virtual void InternalRead(TiXmlElement* element);
	virtual bool InternalContains(const DVector3& position) const
	{
		// 2d object, hence...
		return false;
	}

};
