#include "stdafx.h"
#include "ColourFunction.h"
#include "tinyxml.h"
#include "convert.h"

DCF* DCF::CreateFromXML(TiXmlElement* element)
{
	DCF* dcf;
	int type;
	Convert::StrToInt(element->Attribute("type"), type);
	
	switch (type) 
	{
	case 0:
		dcf = new DCFConstant();
		dcf->Read(element);
		break;
	case 1:
		dcf = new DCFGrid();
		dcf->Read(element);
		break;
	};

	return dcf;
}

void DCFConstant::Read(TiXmlElement* DCFXml)
{
	Convert::StrToCol(DCFXml->Attribute("colour"), mColour);
}

void DCFGrid::Read(TiXmlElement* DCFXml)
{
	Convert::StrToCol(DCFXml->Attribute("colour1"), mColour1);
	Convert::StrToCol(DCFXml->Attribute("colour2"), mColour2);
	Convert::StrToFloat(DCFXml->Attribute("ratio"), mRatio);
	Convert::StrToFloat(DCFXml->Attribute("scale"), mScale);
	Convert::StrToBool(DCFXml->Attribute("checkerboard"), mCheckerboard);
}

DColour DCFGrid::GetColour(const DVector3& rPosition)
{
	DVector3 scaledVector = rPosition / mScale;
	scaledVector += DVector3(0.5, 0.5, 0.5);
	float remainderx = fmod(float(scaledVector.mComponent[0]), 1.f);
	float remaindery = fmod(float(scaledVector.mComponent[1]), 1.f);
	float remainderz = fmod(float(scaledVector.mComponent[2]), 1.f);
	if (remainderx<0) remainderx += 1;
	if (remaindery<0) remaindery += 1;
	if (remainderz<0) remainderz += 1;
	bool col1x = remainderx < mRatio;
	bool col1y = remaindery < mRatio;
	bool col1z = remainderz < mRatio;
	if (mCheckerboard)
	{
		if (((col1x?1:0) + (col1y?1:0) + (col1z?1:0))%2==1)
			return mColour1;
		else
			return mColour2;
	}
	else
	{
		if (col1x && col1y && col1z)
			return mColour1;
		else
			return mColour2;
	}
}
/*
void DCFPerlin::Read(TiXmlElement* DCFXml)
{
	Convert::StrToCol(DCFXml->Attribute("colour1"), mColour1);
	Convert::StrToCol(DCFXml->Attribute("colour2"), mColour2);
	Convert::StrToInt(DCFXml->Attribute("seed"), mSeed);
	Convert::StrToFloat(DCFXml->Attribute("scale"), mScale);
}

DColour DCFPerlin::GetColour(const DVector3& rPosition)
{
	DVector3 scaledVector = rPosition / mScale;
	scaledVector += DVector3(0.25f, 0.25f, 0.25f);
	float remainderx = fmod(scaledVector.mComponent[0], 1);
	float remaindery = fmod(scaledVector.mComponent[1], 1);
	float remainderz = fmod(scaledVector.mComponent[2], 1);
	if (remainderx<0) remainderx += 1;
	if (remaindery<0) remaindery += 1;
	if (remainderz<0) remainderz += 1;
}
*/