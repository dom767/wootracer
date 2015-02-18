#pragma once

#include "Vector3.h"

class DColour
{
public:
	DColour() : mRed(0.f), mGreen(0.f), mBlue(0.f) {}
	DColour(float r, float g, float b) : mRed(r), mGreen(g), mBlue(b) {}
	DColour(const DVector3& vec) : mRed(vec.mComponent[0]), mGreen(vec.mComponent[1]), mBlue(vec.mComponent[2]) {}
	DVector3 GetVector()
	{
		return DVector3(mRed, mGreen, mBlue);
	}
	void operator+=(const DColour& rhs)
	{
		mRed+=rhs.mRed;
		mGreen+=rhs.mGreen;
		mBlue+=rhs.mBlue;
	}
	void operator*=(const DColour& rhs)
	{
		mRed*=rhs.mRed;
		mGreen*=rhs.mGreen;
		mBlue*=rhs.mBlue;
	}
	void operator*=(const float rhs)
	{
		mRed*=rhs;
		mGreen*=rhs;
		mBlue*=rhs;
	}
	void operator/=(const float rhs)
	{
		mRed/=rhs;
		mGreen/=rhs;
		mBlue/=rhs;
	}
	DColour operator*(const DColour& rhs)
	{
		return DColour(mRed*rhs.mRed,
			mGreen*rhs.mGreen,
			mBlue*rhs.mBlue);
	}
	DColour operator/(const float rhs)
	{
		return DColour(mRed/rhs,
			mGreen/rhs,
			mBlue/rhs);
	}
	DColour operator*(const float rhs)
	{
		return DColour(mRed*rhs,
			mGreen*rhs,
			mBlue*rhs);
	}
	DColour operator+(const DColour& rhs)
	{
		return DColour(mRed+rhs.mRed,
			mGreen+rhs.mGreen,
			mBlue+rhs.mBlue);
	}
	float Max()
	{
		float max = mRed>mGreen ? mRed : mGreen;
		max = mBlue>max ? mBlue : max;
		return max;
	}
	void Clamp(DColour& col1, DColour& col2)
	{
		mRed = mRed<col1.mRed ? col1.mRed : mRed>col2.mRed ? col2.mRed : mRed;
		mGreen = mGreen<col1.mGreen ? col1.mGreen : mGreen>col2.mGreen ? col2.mGreen : mGreen;
		mBlue = mBlue<col1.mBlue ? col1.mBlue : mBlue>col2.mBlue ? col2.mBlue : mBlue;
	}

	float mRed;
	float mGreen;
	float mBlue;

};
