#pragma once

#include "Vector3.h"

class DRay
{
public:
	DRay() {}
	DRay(const DVector3 &start, const DVector3 &direction)
		: mStart(start), mDirection(direction), mInvValid(false) {}

	DRay& operator=(const DRay &rhs)
	{
		mStart=rhs.mStart;
		mDirection=rhs.mDirection;
		mInvValid = false;
		return *this;
	}

	void SetStart(const DVector3& start) {mStart=start;}
	void SetDirection(const DVector3& direction) {mDirection=direction;mInvValid = false;}
	void CalcInverse() const
	{
		mInvDirection[0] = 1 / mDirection[0];
		mInvDirection[1] = 1 / mDirection[1];
		mInvDirection[2] = 1 / mDirection[2];
		mInvValid = true;
	}

	const DVector3& GetStart() const {return mStart;}
	const DVector3& GetDirection() const {return mDirection;}
	const DVector3& GetInvDirection() const {if (!mInvValid) {CalcInverse();}return mInvDirection;}
	const bool GetXValPositive() const {return mDirection[0]>0;}
	const bool GetYValPositive() const {return mDirection[1]>0;}
	const bool GetZValPositive() const {return mDirection[2]>0;}
	const float GetRandX() const {return mRandX;}
	const float GetRandY() const {return mRandY;}

private:
	DVector3 mStart;
	DVector3 mDirection;
	float mRandX;
	float mRandY;
	mutable DVector3 mInvDirection;
	mutable bool mInvValid;


};
