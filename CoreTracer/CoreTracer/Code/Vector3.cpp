#include "stdafx.h"
#include "vector3.h"
#include "Matrix4.h"

void DVector3::Mul(const DMatrix4& rhs)
{
	float x = rhs.mVal[0][0] * mComponent[0] + rhs.mVal[1][0] * mComponent[1]  + rhs.mVal[2][0] * mComponent[2] + rhs.mVal[3][0];
	float y = rhs.mVal[0][1] * mComponent[0] + rhs.mVal[1][1] * mComponent[1]  + rhs.mVal[2][1] * mComponent[2] + rhs.mVal[3][1];
	float z = rhs.mVal[0][2] * mComponent[0] + rhs.mVal[1][2] * mComponent[1]  + rhs.mVal[2][2] * mComponent[2] + rhs.mVal[3][2];
	mComponent[0] = x;
	mComponent[1] = y;
	mComponent[2] = z;
}

