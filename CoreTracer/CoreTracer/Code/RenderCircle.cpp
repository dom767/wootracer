#include "stdafx.h"
#include "RenderCircle.h"
#include "Scene.h"
#include "tinyxml.h"
#include "convert.h"
/*
BEGIN_SCENE_OBJECT(DRenderCircle)
REFLECT_VARIABLE(mObjectId)
REFLECT_VARIABLE(mObjectName)
REFLECT_VARIABLE(mSize)
REFLECT_VARIABLE(mNormal)
REFLECT_VARIABLE(mCentre)
REFLECT_VARIABLE(mIgnoreWhileLighting)
END_SCENE_OBJECT()
*/
void DRenderCircle::InternalRead(TiXmlElement* element)
{
	mSize = 1;
	mNormal = DVector3(0,1,0);
//	Convert::StrToFloat(element->Attribute("size"), mSize);
	//Convert::StrToVec3(element->Attribute("normal"), mNormal);

	PostLoad();
}

bool DRenderCircle::InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const
{
	// fast hit test
	if (mNormal.Dot(rRayContext.m_Ray.GetDirection())>0)
		return false;

	float divisor = mNormal.Dot(rRayContext.m_Ray.GetDirection());
	if (divisor==0) return false; // parallel to plane

	float a = -mNormal.Dot(rRayContext.m_Ray.GetStart())/divisor;
	out_Response.mHitPosition = rRayContext.m_Ray.GetStart() + rRayContext.m_Ray.GetDirection() * a;

	// circle test
	if ((out_Response.mHitPosition).MagnitudeSquared()>mSize*mSize)
		return false;

	// ray behind the origin?
	if (a<=0)
		return false;

	// set the normal
	out_Response.mNormal = mNormal;

	// zero the crap out of y component
	out_Response.mHitPosition[1] = 0.0f;

	return true;
}