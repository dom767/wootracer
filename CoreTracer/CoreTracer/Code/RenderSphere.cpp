#include "stdafx.h"
#include "RenderSphere.h"
#include <algorithm>
#include "Scene.h"
#include "tinyxml.h"
#include "convert.h"
/*
BEGIN_SCENE_OBJECT(DRenderSphere)
REFLECT_VARIABLE(mObjectId)
REFLECT_VARIABLE(mObjectName)
REFLECT_VARIABLE(mSize)
REFLECT_VARIABLE(mCentre)
REFLECT_VARIABLE(mIgnoreWhileLighting)
END_SCENE_OBJECT()
*/
void DRenderSphere::InternalRead(TiXmlElement* element)
{
//	Convert::StrToFloat(element->Attribute("size"), mSize);
	mSize = 1;

	PostLoad();
}

bool DRenderSphere::InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const
{
	// fast hit test
	DVector3 oc = rRayContext.m_Ray.GetStart()-DVector3(0,0,0);
	float ocDotDir = oc.Dot(rRayContext.m_Ray.GetDirection());
	float b = 2 * ocDotDir;
	float c = oc.MagnitudeSquared() - mSize*mSize;
	float bb4ac = b*b - 4*c;
	if (bb4ac<0)
		return false;

	float sqrtfbb4ac = sqrtf(bb4ac);
	float h1 = (-b - sqrtfbb4ac) * 0.5f;
	float h2 = (-b + sqrtfbb4ac) * 0.5f;
	if (h2<h1) {float temp=h1;h1=h2;h2=temp;}

	if (rRayContext.m_RequestFlags&RequestBackface)// || out_Response.m_WithinObjectId==GetObjectId())
		//std::find(out_Response.m_WithinObjectId.begin(), out_Response.m_WithinObjectId.end(), GetObjectId())!=out_Response.m_WithinObjectId.end())
	{
		if (h2<0.00001f)
			return false;

		DVector3 HitPos = oc + (rRayContext.m_Ray.GetDirection()*h2); // object space
		DVector3 normal = HitPos.GetNegate();
		normal.Normalise();
		out_Response.mObjectPosition = HitPos + DVector3(0,0,0);
		out_Response.mNormal = normal;
	}
	else
	{
		if (h1<0.00001f)
			return false;

		DVector3 HitPos = oc + (rRayContext.m_Ray.GetDirection()*h1); // object space
		DVector3 normal = HitPos;
		normal.Normalise();
		out_Response.mObjectPosition = HitPos + DVector3(0,0,0);
		out_Response.mNormal = normal;
	}

	// we've reached the end, so we must have hit the sphere
	return true;
}

bool DRenderSphere::InternalContains(const DVector3& position) const
{
	return position.MagnitudeSquared() < 1;
}