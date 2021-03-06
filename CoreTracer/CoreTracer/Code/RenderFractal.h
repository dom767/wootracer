#pragma once

#include "RenderDistance.h"

class DFractalIteration
{
public:
	virtual void Calculate(DVector3& pos, const DVector3& origPos, float& scale)=0;
	virtual void Read(TiXmlElement* element)=0;
	virtual float Estimate(float r, float scale)=0;
};

class DFractalTetra : public DFractalIteration
{
public:
	void Read(TiXmlElement* element);
	void Calculate(DVector3& pos, const DVector3& origPos, float& scale);
	float Estimate(float r, float scale) {return (r - 1) / abs(scale);}

private:
	DMatrix4 mRotate1Matrix, mRotate2Matrix;
	DVector3 mRotate1, mRotate2, mOffset, mPOffset;
	float mScale;
};

class DFractalCuboid : public DFractalIteration
{
public:
	void Read(TiXmlElement* element);
	void Calculate(DVector3& pos, const DVector3& origPos, float& scale);
	float Estimate(float r, float scale) {return (r - 1) / abs(scale);}

private:
	DMatrix4 mRotate1Matrix, mRotate2Matrix;
	DVector3 mRotate1, mRotate2, mOffset, mPOffset;
	float mScale;
};

class DFractalMenger : public DFractalIteration
{
public:
	void Read(TiXmlElement* element);
	void Calculate(DVector3& pos, const DVector3& origPos, float& scale);
	float Estimate(float r, float scale) {return (r - 1) / abs(scale);}

private:
	DMatrix4 mRotate1Matrix, mRotate2Matrix;
	DVector3 mRotate1, mRotate2, mOffset, mPOffset;
	float mScale;
};

class DFractalMandelBox : public DFractalIteration
{
public:
	void Read(TiXmlElement* element);
	void Calculate(DVector3& pos, const DVector3& origPos, float& scale);
	float Estimate(float r, float scale) {return (r - 1) / abs(scale);}

private:
	DMatrix4 mRotate1Matrix;
	DVector3 mRotate1;
	float mScale;
};

class DFractalMandelBulb : public DFractalIteration
{
public:
	void Read(TiXmlElement* element);
	void Calculate(DVector3& pos, const DVector3& origPos, float& scale);
	float Estimate(float r, float scale) {return 0.5f*log(r)*r/scale;}

private:
	DMatrix4 mRotate1Matrix;
	DVector3 mRotate1;
	float mScale;
};

class DRenderFractal : public DRenderDistance
{
public:
	DRenderFractal() : mDEMode(0) {}
	~DRenderFractal()
	{
		for (unsigned int i=0; i<_FractalIterations.size(); i++)
		{
			delete _FractalIterations[i];
		}
		_FractalIterations.clear();
	}

	virtual DRenderObject* InternalClone()
	{
		DRenderFractal* ret = new DRenderFractal();
		return ret;
	}

	virtual float DistanceEstimator(DVector3& pos) const;
protected:
	virtual void InternalRead(TiXmlElement* element);
	virtual DColour DEColour(const DVector3& pos) const;

private:
//	virtual float DistanceEstimator(DVector3& pos);
	float EscapeLength(DVector3& pos, DVector3& modpos, float& modscale, int& iterations) const;
	int mFractalIterations;
	int mColourIterations;
	int mDEMode;
	std::vector<DFractalIteration*> _FractalIterations;
};
