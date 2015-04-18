#include "stdafx.h"
#include "RenderSVO.h"
#include "Scene.h"
#include "tinyxml.h"
#include "convert.h"
#include "Timer.h"

void DRenderSVO::InternalRead(TiXmlElement* element)
{
	Convert::StrToInt(element->Attribute("depth"), mDepth);

	const char* distanceEvaluator = element->Attribute("distancefunction");
	std::string de = "set(distance, sphere(cosx(pos, 1.0), vec(0, 0, 0), 1.0))";
	if (distanceEvaluator) de = distanceEvaluator;
	mDistanceProgram.Create(de);

	mMinimumDistance = 0.001f;
	Convert::StrToFloat(element->Attribute("minimumdistance"), mMinimumDistance);

	Convert::StrToFloat(element->Attribute("distancescale"), mDEScale);
	Convert::StrToVec3(element->Attribute("distanceoffset"), mDEOffset);

	Convert::StrToFloat(element->Attribute("stepsize"), mStepSize);

	DVector3 min = (DVector3(-1,-1,-1) - mDEOffset) / mDEScale;
	DVector3 max = (DVector3(1,1,1) - mDEOffset) / mDEScale;

	LARGE_INTEGER start, freq, end;

	QueryPerformanceCounter(&start);
	QueryPerformanceFrequency(&freq);
	mSVO.CreateSVO(mDistanceProgram, mMinimumDistance, min, max, mStepSize, mDepth);
	QueryPerformanceCounter(&end);
	double time = double(end.QuadPart - start.QuadPart) / double(freq.QuadPart);
	WCHAR dest[256];
	swprintf(dest, 256, L"Time : %f\r\n", time);
	OutputDebugString(dest);

	PostLoad();
}

bool DRenderSVO::InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const
{
	return mSVO.InternalIntersect(rRayContext, out_Response);
}

DSVO::DSVO()
{
	for (int i=0; i<8; i++)
	{
		mChildren[i] = NULL;
	}
}

DSVO::~DSVO()
{
	for (int i=0; i<8; i++)
	{
		delete mChildren[i];
	}
}

void DSVO::CreateSVO(DProgram& program, float minimumDistance, DVector3& min, DVector3& max, float stepSize, int depth)
{
	DVector3 mid = (min+max)*0.5f;

	DFunctionState state;
	state.mPosition = mid;
	program.Run(state);
	float size = (max-mid).Magnitude()*stepSize;
	if (state.mDistance>size)
	{
		mValue = 0;
		return;
	}
	else if (state.mDistance<-size)
	{
		mValue = 1;
		return;
	}


	if (depth==0)
	{
		if (state.mDistance<minimumDistance)
			mValue = 1;
		else
			mValue = 0;
	}
	else
	{
		mValue = -1;

		mChildren[0] = new DSVO();
		mChildren[0]->CreateSVO(program, minimumDistance, DVector3(min[0], min[1], min[2]), DVector3(mid[0], mid[1], mid[2]), stepSize, depth-1);
		mChildren[1] = new DSVO();
		mChildren[1]->CreateSVO(program, minimumDistance, DVector3(mid[0], min[1], min[2]), DVector3(max[0], mid[1], mid[2]), stepSize, depth-1);
		mChildren[2] = new DSVO();
		mChildren[2]->CreateSVO(program, minimumDistance, DVector3(min[0], mid[1], min[2]), DVector3(mid[0], max[1], mid[2]), stepSize, depth-1);
		mChildren[3] = new DSVO();
		mChildren[3]->CreateSVO(program, minimumDistance, DVector3(mid[0], mid[1], min[2]), DVector3(max[0], max[1], mid[2]), stepSize, depth-1);
		mChildren[4] = new DSVO();
		mChildren[4]->CreateSVO(program, minimumDistance, DVector3(min[0], min[1], mid[2]), DVector3(mid[0], mid[1], max[2]), stepSize, depth-1);
		mChildren[5] = new DSVO();
		mChildren[5]->CreateSVO(program, minimumDistance, DVector3(mid[0], min[1], mid[2]), DVector3(max[0], mid[1], max[2]), stepSize, depth-1);
		mChildren[6] = new DSVO();
		mChildren[6]->CreateSVO(program, minimumDistance, DVector3(min[0], mid[1], mid[2]), DVector3(mid[0], max[1], max[2]), stepSize, depth-1);
		mChildren[7] = new DSVO();
		mChildren[7]->CreateSVO(program, minimumDistance, DVector3(mid[0], mid[1], mid[2]), DVector3(max[0], max[1], max[2]), stepSize, depth-1);

		bool full = true;
		bool empty = true;
		for (int i=0; i<8; i++)
		{
			if (mChildren[i]->GetValue()!=1)
				full = false;
			if (mChildren[i]->GetValue()!=0)
				empty = false;
		}

		if (full || empty)
		{
			for (int i=0; i<8; i++) {delete mChildren[i];mChildren[i]=NULL;}
			if (full) mValue = 1;
			else mValue = 0;
		}

	}
}

bool GetSVOCellIntersect(const DRay& ray, DVector3 min, DVector3 max, DVector3& hitPosition, DVector3& outPosition, DVector3& normal, DVector3& outNormal)
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

bool DSVO::InternalIntersectRec(const DRayContext& rRayContext, DCollisionResponse& out_Response, const DVector3& hitPosition, const DVector3& normal, const DVector3& min, const DVector3& max) const
{
	if (mValue==1)
	{
		DVector3 hp2, op2, n2, on2;
		bool hit = GetSVOCellIntersect(rRayContext.m_Ray, min, max, hp2, op2, n2, on2);
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
	else if (mValue==0)
	{
		return false;
	}

	float x0 = min.mComponent[0];
	float x2 = max.mComponent[0];
	float x1 = (x0+x2) / 2;

	float y0 = min.mComponent[1];
	float y2 = max.mComponent[1];
	float y1 = (y0+y2) / 2;

	float z0 = min.mComponent[2];
	float z2 = max.mComponent[2];
	float z1 = (z0+z2) / 2;

	int x = hitPosition.mComponent[0] <= x1 ? 0 : 1;
	int y = hitPosition.mComponent[1] <= y1 ? 0 : 1;
	int z = hitPosition.mComponent[2] <= z1 ? 0 : 1;

	DVector3 newNormal = normal;
	DVector3 newmin, newmax;
	DVector3 newHitPosition = hitPosition;

	while (x>=0 && x<=1 && y>=0 && y<=1 && z>=0 && z<=1)
	{
		newmin.mComponent[0] = x==0 ? x0 : x1;
		newmin.mComponent[1] = y==0 ? y0 : y1;
		newmin.mComponent[2] = z==0 ? z0 : z1;

		newmax.mComponent[0] = x==0 ? x1 : x2;
		newmax.mComponent[1] = y==0 ? y1 : y2;
		newmax.mComponent[2] = z==0 ? z1 : z2;

		int childIdx = x+y*2+z*4;

		// test if this cell is solid
		if (mValue!=0)
		{
			if (mChildren[childIdx]->InternalIntersectRec(rRayContext, out_Response, newHitPosition, newNormal, newmin, newmax))
				return true;
		}

		DVector3 hp2, op2, n2, on2;
		bool collision = GetSVOCellIntersect(rRayContext.m_Ray, newmin, newmax, hp2, op2, n2, on2);
				
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

bool DSVO::InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const
{
	DVector3 min = DVector3(-0.5, -0.5, -0.5), max = DVector3(0.5, 0.5, 0.5);
	DVector3 hitPosition;
	DVector3 outPosition, normal, outNormal;
	
	bool hit = GetSVOCellIntersect(rRayContext.m_Ray, min, max, hitPosition, outPosition, normal, outNormal);
	// or contains
	if (hit)
		return InternalIntersectRec(rRayContext, out_Response, hitPosition, normal, min, max);
	else
	{
		if (rRayContext.m_Ray.GetStart()[0] >= min[0]
		&& rRayContext.m_Ray.GetStart()[1] >= min[1]
		&& rRayContext.m_Ray.GetStart()[2] >= min[2]
		&& rRayContext.m_Ray.GetStart()[0] <= max[0]
		&& rRayContext.m_Ray.GetStart()[1] <= max[1]
		&& rRayContext.m_Ray.GetStart()[2] <= max[2])
			return InternalIntersectRec(rRayContext, out_Response, rRayContext.m_Ray.GetStart(), normal, min, max);
	}

	return false;
}