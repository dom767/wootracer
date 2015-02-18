#include "stdafx.h"
#include "RenderSquare.h"
#include "Scene.h"
#include "tinyxml.h"
#include "convert.h"

void DRenderSquare::InternalRead(TiXmlElement* element)
{
	Convert::StrToFloat(element->Attribute("size"), mSize);
	Convert::StrToVec3(element->Attribute("normal"), mNormal);

	PostLoad();
}

bool DRenderSquare::InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const
{
	// fast hit test
	if (mNormal.Dot(rRayContext.m_Ray.GetDirection())>0)
		return false;

	float a = -mNormal.Dot(rRayContext.m_Ray.GetStart())/mNormal.Dot(rRayContext.m_Ray.GetDirection());
	out_Response.mHitPosition = rRayContext.m_Ray.GetStart() + rRayContext.m_Ray.GetDirection() * a;

	// Square test
	if (!(out_Response.mHitPosition[0]>-0.5 && out_Response.mHitPosition[0]<0.5 &&
		out_Response.mHitPosition[2]>-0.5 && out_Response.mHitPosition[2]<0.5))
		return false;

	// ray behind the origin?
	if (a<=0)
		return false;

	// set the normal
	out_Response.mNormal = mNormal;

	return true;
}