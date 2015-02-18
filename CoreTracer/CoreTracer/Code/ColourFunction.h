#pragma once

#include "SceneObject.h"

class TiXmlElement;

class DCF
{
public:
	virtual ~DCF() {}
	virtual DColour GetColour(const DVector3& Position) = 0;
	static DCF* CreateFromXML(TiXmlElement* element);
	virtual void Read(TiXmlElement* element) = 0;
};

class DCFConstant : public DCF
{
public:
	DCFConstant() {}
	DCFConstant(float constantR, float constantG, float constantB)
		: mColour(constantR, constantG, constantB) {}
	DCFConstant(DColour rgb)
		: mColour(rgb) {}

	DColour GetColour(const DVector3& Position) {return mColour;}
	void Read(TiXmlElement* element);

private:
	DColour mColour;

};

class DCFGrid : public DCF
{
public:
	DCFGrid() {}

	DColour GetColour(const DVector3& rPosition);
	void Read(TiXmlElement* element);

private:
	DColour mColour1;
	DColour mColour2;
	float mScale;
	float mRatio;
	bool mCheckerboard;

};

class DCFPerlin : public DCF
{
public:
	DCFPerlin() {}

	DColour GetColour(const DVector3& rPosition);
	void Read(TiXmlElement* element);

private:
	DColour mColour1;
	DColour mColour2;
	int mSeed;
	float mScale;
	float mValues[256];

};