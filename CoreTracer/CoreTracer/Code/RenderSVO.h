#pragma once

#include "RenderObject.h"

class DSVO
{
public:
	DSVO();
	~DSVO();

	void CreateSVO(DProgram& program, float minimumDistance, DVector3& min, DVector3& max, float stepsize, int depth);
	char GetValue() {return mValue;}
	bool InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const;

private:
	bool InternalIntersectRec(const DRayContext& rRayContext,
		DCollisionResponse& out_Response,
		const DVector3& hitPosition,
		const DVector3& normal,
		const DVector3& min,
		const DVector3& max) const;

	char mValue; // -1 = haschildren, 0 = empty, 1 = full
	DSVO* mChildren[8];

};

class DRenderSVO : public DRenderObject
{
public:
	DRenderSVO()
	{
		mCentre = DVector3(0,0,0);
		mDepth = 4;
		mAABoundingBox = DAABoundingBox(mCentre, mScale[0], mScale[1], mScale[2]);
		mAABoundingBox.TransformBy(mRotation);
	}

	DRenderObject* InternalClone()
	{
		DRenderSVO* ret = new DRenderSVO();
		ret->mDepth = mDepth;
		ret->mDistanceProgram = mDistanceProgram;
		return ret;
	}

	virtual void PostLoad()
	{
		mAABoundingBox = DAABoundingBox(mCentre, mScale[0], mScale[1], mScale[2]);
		mAABoundingBox.TransformBy(mRotation);
	}

	void Read(TiXmlElement* element);

	int mDepth;
	DProgram mDistanceProgram;
	float mMinimumDistance;
	float mDEScale;
	DVector3 mDEOffset;
	DSVO mSVO;
	float mStepSize;

protected:
	virtual bool InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const;
	virtual void InternalRead(TiXmlElement* element);

};
