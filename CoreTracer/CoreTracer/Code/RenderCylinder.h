#pragma once

#include "RenderObject.h"

class DRenderCylinder : public DRenderObject
{
public:
	DRenderCylinder()
	{
		mAABoundingBox = DAABoundingBox(mCentre, mScale[0], mScale[1], mScale[2]);
		mAABoundingBox.TransformBy(mRotation);
	}

	DRenderObject* InternalClone()
	{
		DRenderCylinder* ret = new DRenderCylinder();
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

};
