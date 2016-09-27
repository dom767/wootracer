#include "stdafx.h"
#include "AABoundingBox.h"
#include "Ray.h"
#include "Matrix4.h"

bool DAABoundingBox::Contains(const DVector3 &point) const
{
	DVector3 min = GetMinPoint();
	DVector3 max = GetMaxPoint();
	if (point[0]>min[0]
		&& point[0]<max[0]
		&& point[1]>min[1]
		&& point[1]<max[1]
		&& point[2]>min[2]
		&& point[2]<max[2])
		return true;
	return false;
}

bool DAABoundingBox::Intersects(const DRay& ray, DVector3& out_Start, DVector3& out_End) const
{
	DVector3 min = GetMinPoint();
	DVector3 max = GetMaxPoint();
	float t0=-100000, t1=100000;
	const DVector3& start = ray.GetStart();
	const DVector3& invDirection = ray.GetInvDirection();
	const DVector3& direction = ray.GetDirection();
	float newT0, newT1;

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
		}
        else
		{
			t0=newT1;
			t1=newT0;
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
				t0=newT0;
			if (newT1<t1)
				t1=newT1;
		}
        else
		{
			if (newT1>t0)
				t0=newT1;
			if (newT0<t1)
				t1=newT0;
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
				t0=newT0;
			if (newT1<t1)
				t1=newT1;
		}
        else
		{
			if (newT1>t0)
				t0=newT1;
			if (newT0<t1)
				t1=newT0;
		}
	}

	if (t0>t1)
	{
		/*System::Windows::Forms::MessageBox::Show("does this mean?!?", "What the fuck",
			System::Windows::Forms::MessageBoxButtons::OK,
			System::Windows::Forms::MessageBoxIcon::Exclamation);*/
		return false;
	}

	if (t0<0 && t1<0)
		return false;
	else if (t0<0)
	{
		out_Start = start;
		out_End = start + direction*t1;
		return true;
	}
	else
	{
		out_Start = start + direction*t0;
		out_End = start + direction*t1;
		return true;
	}
}

const void DAABoundingBox::TransformBy(const DMatrix4& matrix)
{
	DVector3 points[8];
	int i;
	float x = mExtents[0], y = mExtents[1], z = mExtents[2];
	
	points[0] = DVector3(-x, -y, -z);
	points[1] = DVector3(-x, -y, z);
	points[2] = DVector3(-x, y, -z);
	points[3] = DVector3(-x, y, z);
	points[4] = DVector3(x, -y, -z);
	points[5] = DVector3(x, -y, z);
	points[6] = DVector3(x, y, -z);
	points[7] = DVector3(x, y, z);

	for (int i=0; i<8; i++)
	{
		points[i] = matrix * points[i];
	}

	DVector3 min, max;
	min = max = points[0];

	for (i=1; i<8; i++)
	{
		if (points[i][0]<min[0]) min[0] = points[i][0];
		if (points[i][0]>max[0]) max[0] = points[i][0];

		if (points[i][1]<min[1]) min[1] = points[i][1];
		if (points[i][1]>max[1]) max[1] = points[i][1];

		if (points[i][2]<min[2]) min[2] = points[i][2];
		if (points[i][2]>max[2]) max[2] = points[i][2];
	}

	mExtents = max;
}
