#pragma once

#include "RenderObject.h"

class DDistFunc;

class DRenderDistance : public DRenderObject
{
public:
	DRenderDistance() : mMinimumDistance(0.001f), mDEScale(1.f), mDEOffset(0.0f,0.f,0.f), mDEIterations(200), mDEExtents(1.f,1.f,1.f), mStepSize(0.7f)	
	{
		mAABoundingBox = DAABoundingBox(mCentre, mScale[0], mScale[1], mScale[2]);
		mAABoundingBox.TransformBy(mRotation);
	}

	DRenderObject* InternalClone()
	{
		DRenderDistance* ret = new DRenderDistance();
		return ret;
	}
	virtual void PostLoad()
	{
		mAABoundingBox = DAABoundingBox(mCentre, mScale[0], mScale[1], mScale[2]);
		mAABoundingBox.TransformBy(mRotation);
	}
	void Read(TiXmlElement* element);

protected:
	virtual bool InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const;
	virtual void InternalRead(TiXmlElement* element);

private:
	float DistanceEstimator(DVector3& pos) const;

	DProgram mDistanceProgram;
	float mMinimumDistance;	
	float mDEScale;
	DVector3 mDEOffset;
	int mDEIterations;
	DVector3 mDEExtents;
	float mStepSize;
};
