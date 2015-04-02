#include "stdafx.h"
#include "RenderCube.h"
#include "Scene.h"
#include "tinyxml.h"
#include "convert.h"
/*
BEGIN_SCENE_OBJECT(DRenderCube)
END_SCENE_OBJECT()
*/
void DRenderCube::InternalRead(TiXmlElement* element)
{
	Convert::StrToFloat(element->Attribute("size"), mSideLength);

	PostLoad();
}

bool DRenderCube::InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const
{
	DVector3 min = DVector3(-0.5, -0.5, -0.5);
	DVector3 max = DVector3(0.5, 0.5, 0.5);
	float t0=-100000, t1=100000;
	const DVector3& start = rRayContext.m_Ray.GetStart();
	const DVector3& invDirection = rRayContext.m_Ray.GetInvDirection();
	const DVector3& direction = rRayContext.m_Ray.GetDirection();
	float newT0, newT1;
	DVector3 normal0, normal1;

	// Check for perpendicular ray
	if (direction[0]==0)
	{
		// if we're perpendicular to X axis then see if the X value is inside the box
		if (start[0]<min[0] || start[0]>max[0])
			return false; // guess not
	}
	else
	{
		// calculate entry and exit points on the two X planes
		newT0 = (min[0]-start[0])*invDirection[0];
		newT1 = (max[0]-start[0])*invDirection[0];
		
		// ray is moving from min to max update t0, t1 if necessary
		if (newT1>newT0)
		{
			t0=newT0;
			t1=newT1;
			normal0 = DVector3(-1, 0, 0);
			normal1 = DVector3(1, 0, 0);
		}
        else
		{
			t0=newT1;
			t1=newT0;
			normal0 = DVector3(1, 0, 0);
			normal1 = DVector3(-1, 0, 0);
		}
	}

	// Check for perpendicular ray
	if (direction[1]==0)
	{
		// if we're perpendicular to Y axis then see if the Y value is inside the box
		if (start[1]<min[1] || start[1]>max[1])
			return false; // guess not
	}
	else
	{
		// calculate entry and exit points on the two Y planes
		newT0 = (min[1]-start[1])*invDirection[1];
		newT1 = (max[1]-start[1])*invDirection[1];
		
		// ray is moving from min to max update t0, t1 if necessary
		if (newT1>newT0)
		{
			if (newT0>t0)
			{
				t0=newT0;
				normal0 = DVector3(0, -1, 0);
			}
			if (newT1<t1)
			{
				t1=newT1;
				normal1 = DVector3(0, 1, 0);
			}
		}
        else
		{
			if (newT1>t0)
			{
				t0=newT1;
				normal0 = DVector3(0, 1, 0);
			}
			if (newT0<t1)
			{
				t1=newT0;
				normal1 = DVector3(0, -1, 0);
			}
		}
	}

	// Check for perpendicular ray
	if (direction[2]==0)
	{
		// if we're perpendicular to Z axis then see if the Z value is inside the box
		if (start[2]<min[2] || start[2]>max[2])
			return false; // guess not
	}
	else
	{
		// calculate entry and exit points on the two Z planes
		newT0 = (min[2]-start[2])*invDirection[2];
		newT1 = (max[2]-start[2])*invDirection[2];
		
		// ray is moving from min to max update t0, t1 if necessary
		if (newT1>newT0)
		{
			if (newT0>t0)
			{
				t0=newT0;
				normal0 = DVector3(0, 0, -1);
			}
			if (newT1<t1)
			{
				t1=newT1;
				normal1 = DVector3(0, 0, 1);
			}
		}
        else
		{
			if (newT1>t0)
			{
				t0=newT1;
				normal0 = DVector3(0, 0, 1);
			}
			if (newT0<t1)
			{
				t1=newT0;
				normal1 = DVector3(0, 0, -1);
			}
		}
	}

	if (t0>t1)
	{
		return false;
	}
	if (t0<0 && t1<0)
	{
		return false;
	}
	if (t0>0 && !((rRayContext.m_RequestFlags&RequestBackface)>0))
	{
		if (t0<0.00001f)
			return false;

		out_Response.mHitPosition = start + (rRayContext.m_Ray.GetDirection()*t0); // object space
		out_Response.mNormal = normal0;
		return true;
	}
	if (t1>0 && rRayContext.m_RequestFlags&RequestBackface)
	{
		if (t1<0.00001f)
			return false;
		out_Response.mHitPosition = start + (rRayContext.m_Ray.GetDirection()*t1); // object space
		out_Response.mNormal = normal1;
		return true;
	}

	return false;
}