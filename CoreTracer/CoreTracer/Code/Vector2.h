#pragma once

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

	float x, y;
};