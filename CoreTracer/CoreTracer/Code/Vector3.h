#ifndef VECTOR3_H
#define VECTOR3_H

#include <math.h>

enum EAxis
{
	XAxis=0,
	YAxis=1,
	ZAxis=2
};

#define MAX(a,b) ((a>b) ? a : b)
#define clamp(val, min, max) (val<min) ? min : (val>max) ? max : val
#define PI 3.14159265f

class DVector3
{
public:
	DVector3() {mComponent[0]=mComponent[1]=mComponent[2]=0;}
	DVector3(float x, float y, float z) {mComponent[0]=x;mComponent[1]=y;mComponent[2]=z;}
	DVector3 operator-(const DVector3 rhs) const
	{
		return DVector3(mComponent[0]-rhs.mComponent[0], mComponent[1]-rhs.mComponent[1], mComponent[2]-rhs.mComponent[2]);
	}
	DVector3 operator+(const DVector3 rhs) const
	{
		return DVector3(mComponent[0]+rhs.mComponent[0], mComponent[1]+rhs.mComponent[1], mComponent[2]+rhs.mComponent[2]);
	}
	DVector3 operator*(const float rhs) const
	{
		return DVector3(mComponent[0]*rhs, mComponent[1]*rhs, mComponent[2]*rhs);
	}
	DVector3 operator*(const DVector3& rhs) const
	{
		return DVector3(mComponent[0]*rhs[0], mComponent[1]*rhs[1], mComponent[2]*rhs[2]);
	}
	DVector3 operator/(const float rhs) const
	{
		return DVector3(mComponent[0]/rhs, mComponent[1]/rhs, mComponent[2]/rhs);
	}
	DVector3 operator/(const DVector3& rhs) const
	{
		return DVector3(mComponent[0]/rhs[0], mComponent[1]/rhs[1], mComponent[2]/rhs[2]);
	}
	void operator*=(const float rhs)
	{
		mComponent[0] *= rhs;
		mComponent[1] *= rhs;
		mComponent[2] *= rhs;
	}
	void operator*=(const DVector3& rhs)
	{
		mComponent[0] *= rhs[0];
		mComponent[1] *= rhs[1];
		mComponent[2] *= rhs[2];
	}
	void operator/=(const float rhs)
	{
		mComponent[0] /= rhs;
		mComponent[1] /= rhs;
		mComponent[2] /= rhs;
	}
	void operator/=(const DVector3& rhs)
	{
		mComponent[0] /= rhs[0];
		mComponent[1] /= rhs[1];
		mComponent[2] /= rhs[2];
	}
	float& operator[](const int rhs)
	{
		return mComponent[rhs];
	}
	const float operator[](const int rhs) const
	{
		return mComponent[rhs];
	}
	void operator+=(const DVector3& rhs)
	{
		mComponent[0] += rhs.mComponent[0];
		mComponent[1] += rhs.mComponent[1];
		mComponent[2] += rhs.mComponent[2];
	}
	void operator-=(const DVector3& rhs)
	{
		mComponent[0] -= rhs.mComponent[0];
		mComponent[1] -= rhs.mComponent[1];
		mComponent[2] -= rhs.mComponent[2];
	}
	void Normalise()
	{
		float invmag = 1.f/sqrtf(mComponent[0]*mComponent[0]+mComponent[1]*mComponent[1]+mComponent[2]*mComponent[2]);
		mComponent[0] *= invmag;
		mComponent[1] *= invmag;
		mComponent[2] *= invmag;
	}
	DVector3 Cross(const DVector3& rhs) const
	{
		DVector3 ret;
		ret.mComponent[0] = mComponent[1]*rhs.mComponent[2] - mComponent[2]*rhs.mComponent[1];
		ret.mComponent[1] = mComponent[2]*rhs.mComponent[0] - mComponent[0]*rhs.mComponent[2];
		ret.mComponent[2] = mComponent[0]*rhs.mComponent[1] - mComponent[1]*rhs.mComponent[0];
		return ret;
	}
	float Dot(const DVector3& rhs) const
	{
		return mComponent[0]*rhs.mComponent[0] + mComponent[1]*rhs.mComponent[1] + mComponent[2]*rhs.mComponent[2];
	}
	float Magnitude()
	{
		return sqrtf(mComponent[0]*mComponent[0]+mComponent[1]*mComponent[1]+mComponent[2]*mComponent[2]);
	}
	float MagnitudeSquared()
	{
		return mComponent[0]*mComponent[0]+mComponent[1]*mComponent[1]+mComponent[2]*mComponent[2];
	}
	float GetBiggestComponent()
	{
		return MAX(mComponent[0], MAX(mComponent[1], mComponent[2]));
	}
	float mComponent[3];
	DVector3 GetNegate()
	{
		return DVector3(-mComponent[0], -mComponent[1], -mComponent[2]);
	}
	void Negate()
	{
		mComponent[0] = -mComponent[0];
		mComponent[1] = -mComponent[1];
		mComponent[2] = -mComponent[2];
	}
};

#endif // DVECTOR3_H