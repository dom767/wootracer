#pragma once

#include "RenderObject.h"

class DDistFunc;

class DRenderDistance : public DRenderObject
{
public:
	DRenderDistance() : mMinimumDistance(0.001f), mDEScale(1.f), mDEOffset(0.0f,0.f,0.f), mDEIterations(200), mDEExtents(1.f,1.f,1.f), mStepSize(0.7f)	
	{
		mAABoundingBox = DAABoundingBox(mCentre, mDEExtents[0]*mScale[0], mDEExtents[1]*mScale[1], mDEExtents[2]*mScale[2]);
		mAABoundingBox.TransformBy(mRotation);
	}

	DRenderObject* InternalClone()
	{
		DRenderDistance* ret = new DRenderDistance();
		return ret;
	}
	virtual void PostLoad()
	{
		mAABoundingBox = DAABoundingBox(mCentre, mDEExtents[0]*mScale[0], mDEExtents[1]*mScale[1], mDEExtents[2]*mScale[2]);
		mAABoundingBox.TransformBy(mRotation);
	}
	void Read(TiXmlElement* element);

protected:
	virtual bool InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const;
	virtual void InternalRead(TiXmlElement* element);

private:
	float DistanceEstimator(DVector3& pos) const;
	bool GetCollisionPoint(DVector3 &out_Point, const DVector3& start, const DVector3& end, const DVector3& direction) const;
	bool GetBackfaceCollisionPoint(DVector3 &out_Point, const DVector3& start, const DVector3& end, const DVector3& direction) const;
	DVector3 GetNormal(const DVector3& position, const DVector3& direction) const;
	void Nudge(DVector3& out_Position, const DVector3& normal) const;
	void DeNudge(DVector3& out_Position, const DVector3& normal) const;

	DProgram mDistanceProgram;
	float mMinimumDistance;	
	float mDEScale;
	DVector3 mDEOffset;
	int mDEIterations;
	DVector3 mDEExtents;
	float mStepSize;
};
