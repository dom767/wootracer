#pragma once

#include "RenderObject.h"

class DRenderSphere : public DRenderObject
{
//	DECLARE_SCENE_OBJECT()

public:
	DRenderSphere() {}
	virtual void PostLoad()
	{
		float aascale = mScale.GetBiggestComponent();
		mAABoundingBox = DAABoundingBox(mCentre, mScale[0], mScale[1], mScale[2]);
		mAABoundingBox.TransformBy(mRotation);
	}
	void Read(TiXmlElement* element);
	DRenderObject* InternalClone()
	{
		DRenderSphere* ret = new DRenderSphere();
		//ret->mSize = mSize;
		return NULL;//ret;
	}

	float mSize;

protected:
	virtual bool InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const;
	virtual void InternalRead(TiXmlElement* element);
	virtual bool InternalContains(const DVector3& position) const;

};
