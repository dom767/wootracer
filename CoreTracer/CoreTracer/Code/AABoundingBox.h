#pragma once

#include "Vector3.h"

class DRay;
class DMatrix4;

/// Axis aligned bounding box implementation
class DAABoundingBox
{
public:
	DAABoundingBox() {}
	DAABoundingBox(const DVector3 centre, const float width, const float height, const float depth) :
		mCentre(centre),
		mExtents(width, height, depth) {}
	DAABoundingBox(const DVector3 &minExtents, const DVector3 &maxExtents) :
		mCentre((minExtents+maxExtents)*0.5f),
		mExtents(mCentre[0]-minExtents[0],
			mCentre[1]-minExtents[1],
			mCentre[2]-minExtents[2]) {}

	bool LessThanPlane(const EAxis axis, const float position) const
	{
		if (mCentre[axis]-mExtents[axis] < position)
			return true;
		return false;
	}
	bool GreaterThanPlane(const EAxis axis, const float position) const
	{
		if (mCentre[axis]+mExtents[axis] >= position)
			return true;
		return false;
	}
	bool IntersectsPlane(const EAxis axis, const float position) const
	{
		if (mCentre[axis]-mExtents[axis] < position
			&& mCentre[axis]+mExtents[axis] >= position)
		{
			return true;
		}
		return false;
	}

	const DVector3& GetCentre() const {return mCentre;}
	const DVector3& GetExtents() const {return mExtents;}
	void SetCentre(const DVector3& centre) {mCentre=centre;}
	void SetExtents(const DVector3& extents) {mExtents=extents;}

	const DVector3 GetMinPoint() const {return mCentre-mExtents;}
	const DVector3 GetMaxPoint() const {return mCentre+mExtents;}
	bool Contains(const DVector3& point) const;

	const void TransformBy(const DMatrix4& matrix);

	bool Intersects(const DRay& ray, DVector3& out_Start, DVector3& out_End) const;

private:
	DVector3 mCentre;
	DVector3 mExtents;

};