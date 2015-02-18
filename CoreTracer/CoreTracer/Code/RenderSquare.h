#pragma once

#include "RenderObject.h"

class DRenderSquare : public DRenderObject
{
public:
	DRenderSquare()
	{
		mCentre = DVector3(0,0,0);
		mNormal = DVector3(0,1,0);
		mSize = 0.5;
		mAABoundingBox = DAABoundingBox(mCentre, mScale[0]*mSize, mScale[1]*0.01f, mScale[2]*mSize);
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
		mAABoundingBox = DAABoundingBox(mCentre, mScale[0]*mSize, mScale[1]*0.01f, mScale[2]*mSize);
		mAABoundingBox.TransformBy(mRotation);
	}

	DRenderObject* InternalClone()
	{
		DRenderSquare* ret = new DRenderSquare();
		ret->mSize = mSize;
		ret->mNormal = mNormal;
		return ret;
	}

	float mSize;
	DVector3 mNormal;

protected:
	virtual bool InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const;
	virtual void InternalRead(TiXmlElement* element);

};
