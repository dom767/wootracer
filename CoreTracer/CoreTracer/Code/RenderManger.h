#pragma once

#include "RenderObject.h"

class DRenderManger : public DRenderObject
{
public:
	DRenderManger()
	{
		mCentre = DVector3(0,0,0);
		mIterations = 4;
		mAABoundingBox = DAABoundingBox(mCentre, mScale[0], mScale[1], mScale[2]);
		mAABoundingBox.TransformBy(mRotation);
	}

	DRenderObject* InternalClone()
	{
		DRenderManger* ret = new DRenderManger();
		ret->mIterations = mIterations;
		return ret;
	}

	virtual void PostLoad()
	{
		mAABoundingBox = DAABoundingBox(mCentre, mScale[0], mScale[1], mScale[2]);
		mAABoundingBox.TransformBy(mRotation);
	}

	void Read(TiXmlElement* element);

	int mIterations;
	int mPattern[27];

protected:
	virtual bool InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const;
	virtual void InternalRead(TiXmlElement* element);

private:
	bool InternalIntersectRec(const DRayContext& rRayContext, DCollisionResponse& out_Response, const DVector3& hitPosition, const DVector3& normal, const DVector3& min, const DVector3& max, const int iterations) const;

};
