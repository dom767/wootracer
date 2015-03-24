#pragma once
#include <math.h>

class DVector2
{
public:
	DVector2() {}
	DVector2(float ix, float iy) : x(ix), y(iy) {}

	DVector2 operator+(const DVector2& rhs) const
	{
		return DVector2(x+rhs.x, y+rhs.y);
	}

	DVector2 operator*(const float rhs) const
	{
		return DVector2(x*rhs, y*rhs);
	}

	float GetLength() {return sqrtf(x*x+y*y);}
	float GetLengthSq() {return x*x+y*y;}
	void Normalise() {float mag = GetLength();x/=mag;y/=mag;}

	float GetDot(DVector2 arg) {return x*arg.x + y*arg.y;}

	float x, y;
};