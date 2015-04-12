#include "stdafx.h"
#include "RenderCylinder.h"
#include "Scene.h"
#include "tinyxml.h"
#include "convert.h"

void DRenderCylinder::InternalRead(TiXmlElement* element)
{
	PostLoad();
}

bool DRenderCylinder::InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const
{
	float t0, t1;
	const DVector3& start = rRayContext.m_Ray.GetStart();
	const DVector3& direction = rRayContext.m_Ray.GetDirection();

	// a=xD2+yD2, b=2xExD+2yEyD, and c=xE2+yE2-1. 
	float a = direction.mComponent[0] * direction.mComponent[0]
		+ direction.mComponent[2] * direction.mComponent[2];

	float b = 2 * start.mComponent[0] * direction.mComponent[0]
		+ 2 * start.mComponent[2] * direction.mComponent[2];

	float c = start.mComponent[0] * start.mComponent[0]
		+ start.mComponent[2] * start.mComponent[2]
		- 1;

	float b24ac = b*b - 4*a*c;
	if (b24ac<0)
		return false;

	float sqb24ac = sqrtf(b24ac);
	t0 = (-b + sqb24ac) / (2 * a);
	t1 = (-b - sqb24ac) / (2 * a);

	if (t0>t1) {float tmp = t0;t0=t1;t1=tmp;}

	float y0 = start.mComponent[1] + t0 * direction.mComponent[1];
	float y1 = start.mComponent[1] + t1 * direction.mComponent[1];

	if ((rRayContext.m_RequestFlags&RequestBackface)==0)
	{
		if (y0<-1)
		{
			if (y1<-1)
				return false;
			else
			{
				// hit the cap
				float th = t0 + (t1-t0) * (y0+1) / (y0-y1);
				if (th<=0) return false;
		
				out_Response.mHitPosition = start + (direction*th);
				out_Response.mNormal = DVector3(0, -1, 0);
				return true;
			}
		}
		else if (y0>=-1 && y0<=1)
		{
			// hit the cylinder bit
			if (t0<=0) return false;
		
			out_Response.mHitPosition = start + (direction*t0);
			out_Response.mNormal = DVector3(out_Response.mHitPosition.mComponent[0], 0, out_Response.mHitPosition.mComponent[2]);
			out_Response.mNormal.Normalise();
			return true;
		}
		else if (y0>1)
		{
			if (y1>1)
				return false;
			else
			{
				// hit the cap
				float th = t0 + (t1-t0) * (y0-1) / (y0-y1);
				if (th<=0) return false;

				out_Response.mHitPosition = start + (direction*th);
				out_Response.mNormal = DVector3(0, 1, 0);
				return true;
			}
		}
	}
	else // backface
	{
		if (y1<-1)
		{
			if (y0<-1)
				return false;
			else
			{
				// hit the cap
				float th = t0 + (t1-t0) * (y0+1) / (y0-y1);
				if (th<=0) return false;
		
				out_Response.mHitPosition = start + (direction*th);
				out_Response.mNormal = DVector3(0, 1, 0);
				return true;
			}
		}
		else if (y1>=-1 && y1<=1)
		{
			// hit the cylinder bit
			if (t1<=0) return false;
		
			out_Response.mHitPosition = start + (direction*t1);
			out_Response.mNormal = DVector3(-out_Response.mHitPosition.mComponent[0], 0, -out_Response.mHitPosition.mComponent[2]);
			out_Response.mNormal.Normalise();
			return true;
		}
		else if (y1>1)
		{
			if (y0>1)
				return false;
			else
			{
				// hit the cap
				float th = t0 + (t1-t0) * (y0-1) / (y0-y1);
				if (th<=0) return false;

				out_Response.mHitPosition = start + (direction*th);
				out_Response.mNormal = DVector3(0, -1, 0);
				return true;
			}
		}
	}

	return false;
}