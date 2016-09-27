#include "stdafx.h"
#include "RenderManger.h"
#include "Scene.h"
#include "tinyxml.h"
#include "convert.h"

void DRenderManger::InternalRead(TiXmlElement* element)
{
	Convert::StrToInt(element->Attribute("iterations"), mIterations);
	const char* pattern = element->Attribute("pattern");
	for (int i=0; i<27; i++)
	{
		mPattern[i] = pattern[i*3] == '1' ? 1 : 0;
	}
	PostLoad();
}

bool GetCellIntersect(const DRay& ray, DVector3 min, DVector3 max, DVector3& hitPosition, DVector3& outPosition, DVector3& normal, DVector3& outNormal)
{
	float t0=-100000, t1=100000;
	const DVector3& start = ray.GetStart();
	const DVector3& invDirection = ray.GetInvDirection();
	const DVector3& direction = ray.GetDirection();
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
	else if (t0<0 && t1>=0)
	{
		outPosition = start + (ray.GetDirection()*t1);
		outNormal = normal1;
		return false;
	}
	else if (t0>=0)
	{
		hitPosition = start + (ray.GetDirection()*t0);
		outPosition = start + (ray.GetDirection()*t1);
		normal = normal0;
		outNormal = normal1;
		return true;
	}

	return false;
}

bool DRenderManger::InternalIntersectRec(const DRayContext& rRayContext, DCollisionResponse& out_Response, const DVector3& hitPosition, const DVector3& normal, const DVector3& min, const DVector3& max, const int iterations) const
{
	if (iterations==0)
	{
		DVector3 hp2, op2, n2, on2;
		bool hit = GetCellIntersect(rRayContext.m_Ray, min, max, hp2, op2, n2, on2);
		if (hit)
		{
			out_Response.mObjectPosition = hp2;
			out_Response.mNormal = n2;
			out_Response.mColour = DColour(1,1,1);
			return true;
		}
		else
			return false;
	}

	float x0 = min.mComponent[0];
	float x3 = max.mComponent[0];
	float x1 = (x0*2+x3) / 3;
	float x2 = (x0+x3*2) / 3;

	float y0 = min.mComponent[1];
	float y3 = max.mComponent[1];
	float y1 = (y0*2+y3) / 3;
	float y2 = (y0+y3*2) / 3;

	float z0 = min.mComponent[2];
	float z3 = max.mComponent[2];
	float z1 = (z0*2+z3) / 3;
	float z2 = (z0+z3*2) / 3;

	int x = hitPosition.mComponent[0] <= x1 ? 0 :
		hitPosition.mComponent[0] <= x2 ? 1 : 2;
	int y = hitPosition.mComponent[1] <= y1 ? 0 :
		hitPosition.mComponent[1] <= y2 ? 1 : 2;
	int z = hitPosition.mComponent[2] <= z1 ? 0 :
		hitPosition.mComponent[2] <= z2 ? 1 : 2;

	DVector3 newNormal = normal;
	DVector3 newmin, newmax;
	DVector3 newHitPosition = hitPosition;

	while (x>=0 && x<=2 && y>=0 && y<=2 && z>=0 && z<=2)
	{
		newmin.mComponent[0] = x==0 ? x0 : x==1 ? x1 : x2;
		newmin.mComponent[1] = y==0 ? y0 : y==1 ? y1 : y2;
		newmin.mComponent[2] = z==0 ? z0 : z==1 ? z1 : z2;

		newmax.mComponent[0] = x==0 ? x1 : x==1 ? x2 : x3;
		newmax.mComponent[1] = y==0 ? y1 : y==1 ? y2 : y3;
		newmax.mComponent[2] = z==0 ? z1 : z==1 ? z2 : z3;

		// test if this cell is solid
		if (mPattern[(x+z*3+y*9)]>0)
		{
			if (InternalIntersectRec(rRayContext, out_Response, newHitPosition, newNormal, newmin, newmax, iterations-1))
				return true;
		}

		DVector3 hp2, op2, n2, on2;
		bool collision = GetCellIntersect(rRayContext.m_Ray, newmin, newmax, hp2, op2, n2, on2);
				
		if (on2.mComponent[0]==1) {x++;}
		else if (on2.mComponent[0]==-1) {x--;}
		else if (on2.mComponent[1]==1) {y++;}
		else if (on2.mComponent[1]==-1) {y--;}
		else if (on2.mComponent[2]==1) {z++;}
		else if (on2.mComponent[2]==-1) {z--;}
		else return false; // probably an internal ray start, prevent infinite loop

		newNormal = on2;
		newNormal.mComponent[0] = -newNormal.mComponent[0];
		newNormal.mComponent[1] = -newNormal.mComponent[1];
		newNormal.mComponent[2] = -newNormal.mComponent[2];
		newHitPosition = op2;
	}

	return false;
}

bool DRenderManger::InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const
{
	DVector3 min = DVector3(-0.5, -0.5, -0.5), max = DVector3(0.5, 0.5, 0.5);
	DVector3 hitPosition;
	DVector3 outPosition, normal, outNormal;
	
	bool hit = GetCellIntersect(rRayContext.m_Ray, min, max, hitPosition, outPosition, normal, outNormal);
	// or contains
	if (hit)
		return InternalIntersectRec(rRayContext, out_Response, hitPosition, normal, min, max, mIterations);
	else
	{
		if (rRayContext.m_Ray.GetStart()[0] >= min[0]
		&& rRayContext.m_Ray.GetStart()[1] >= min[1]
		&& rRayContext.m_Ray.GetStart()[2] >= min[2]
		&& rRayContext.m_Ray.GetStart()[0] <= max[0]
		&& rRayContext.m_Ray.GetStart()[1] <= max[1]
		&& rRayContext.m_Ray.GetStart()[2] <= max[2])
			return InternalIntersectRec(rRayContext, out_Response, rRayContext.m_Ray.GetStart(), normal, min, max, mIterations);
	}

	return false;
}

bool DRenderManger::InternalContains(const DVector3& position) const
{
	return InternalContainsRec(position, DVector3(-0.5, -0.5, -0.5), DVector3(0.5, 0.5, 0.5), mIterations);
}

bool DRenderManger::InternalContainsRec(const DVector3& position, const DVector3& min, const DVector3& max, const int iterations) const
{
	if (iterations==0)
	{
		return true;
	}

	float x0 = min.mComponent[0];
	float x3 = max.mComponent[0];
	float x1 = (x0*2+x3) / 3;
	float x2 = (x0+x3*2) / 3;

	float y0 = min.mComponent[1];
	float y3 = max.mComponent[1];
	float y1 = (y0*2+y3) / 3;
	float y2 = (y0+y3*2) / 3;

	float z0 = min.mComponent[2];
	float z3 = max.mComponent[2];
	float z1 = (z0*2+z3) / 3;
	float z2 = (z0+z3*2) / 3;

	int x = position.mComponent[0] <= x1 ? 0 :
		position.mComponent[0] <= x2 ? 1 : 2;
	int y = position.mComponent[1] <= y1 ? 0 :
		position.mComponent[1] <= y2 ? 1 : 2;
	int z = position.mComponent[2] <= z1 ? 0 :
		position.mComponent[2] <= z2 ? 1 : 2;

	DVector3 newmin, newmax;

	newmin.mComponent[0] = x==0 ? x0 : x==1 ? x1 : x2;
	newmin.mComponent[1] = y==0 ? y0 : y==1 ? y1 : y2;
	newmin.mComponent[2] = z==0 ? z0 : z==1 ? z1 : z2;

	newmax.mComponent[0] = x==0 ? x1 : x==1 ? x2 : x3;
	newmax.mComponent[1] = y==0 ? y1 : y==1 ? y2 : y3;
	newmax.mComponent[2] = z==0 ? z1 : z==1 ? z2 : z3;

	if (mPattern[(x+z*3+y*9)]>0)
	{
		return InternalContainsRec(position, newmin, newmax, iterations-1);
	}

	return false;
}