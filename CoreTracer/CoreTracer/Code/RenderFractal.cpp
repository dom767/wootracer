#include "stdafx.h"
#include "RenderFractal.h"
#include "tinyxml.h"
#include "convert.h"

void DFractalTetra::Read(TiXmlElement* element)
{
	Convert::StrToVec3(element->Attribute("rotate1"), mRotate1);
	Convert::StrToVec3(element->Attribute("rotate2"), mRotate2);
	Convert::StrToVec3(element->Attribute("offset"), mOffset);
	Convert::StrToFloat(element->Attribute("scale"), mScale);
	mRotate1Matrix.MakeFromRPY(mRotate1[0],mRotate1[1],mRotate1[2]);
	mRotate2Matrix.MakeFromRPY(mRotate2[0],mRotate2[1],mRotate2[2]);
	mPOffset = mOffset*(mScale*0.5f);
}

void DFractalTetra::Calculate(DVector3& pos, const DVector3& origPos, float& scale)
{
	float tmp;

	pos.Mul(mRotate1Matrix);

	if(pos[0]+pos[1]<0){tmp=-pos[1];pos[1]=-pos[0];pos[0]=tmp;}
	if(pos[0]+pos[2]<0){tmp=-pos[2];pos[2]=-pos[0];pos[0]=tmp;}
	if(pos[1]+pos[2]<0){tmp=-pos[2];pos[2]=-pos[1];pos[1]=tmp;}
      
	pos.Mul(mRotate2Matrix);

	pos = pos*mScale - mPOffset;
	scale *= mScale;
}

void DFractalCuboid::Read(TiXmlElement* element)
{
	Convert::StrToVec3(element->Attribute("rotate1"), mRotate1);
	Convert::StrToVec3(element->Attribute("rotate2"), mRotate2);
	Convert::StrToVec3(element->Attribute("offset"), mOffset);
	Convert::StrToFloat(element->Attribute("scale"), mScale);
	mRotate1Matrix.MakeFromRPY(mRotate1[0],mRotate1[1],mRotate1[2]);
	mRotate2Matrix.MakeFromRPY(mRotate2[0],mRotate2[1],mRotate2[2]);
	mPOffset = mOffset*(mScale*0.5f);
}

void DFractalCuboid::Calculate(DVector3& pos, const DVector3& origPos, float& scale)
{
	pos.Mul(mRotate1Matrix);

	pos[0] = abs(pos[0]);
	pos[1] = abs(pos[1]);
	pos[2] = abs(pos[2]);      

	pos.Mul(mRotate2Matrix);

	pos = pos*mScale - mPOffset;
	scale *= mScale;
}

void DFractalMenger::Read(TiXmlElement* element)
{
	Convert::StrToVec3(element->Attribute("rotate1"), mRotate1);
	Convert::StrToVec3(element->Attribute("rotate2"), mRotate2);
	Convert::StrToVec3(element->Attribute("offset"), mOffset);
	Convert::StrToFloat(element->Attribute("scale"), mScale);
	mRotate1Matrix.MakeFromRPY(mRotate1[0],mRotate1[1],mRotate1[2]);
	mRotate2Matrix.MakeFromRPY(mRotate2[0],mRotate2[1],mRotate2[2]);
	mPOffset = mOffset*(mScale-1);
}

void DFractalMenger::Calculate(DVector3& pos, const DVector3& origPos, float& scale)
{
	pos.Mul(mRotate1Matrix);

	float tmp;

	pos.SetAbs();
	if(pos[0]-pos[1]<0){tmp=pos[1];pos[1]=pos[0];pos[0]=tmp;}
	if(pos[0]-pos[2]<0){tmp=pos[2];pos[2]=pos[0];pos[0]=tmp;}
	if(pos[1]-pos[2]<0){tmp=pos[2];pos[2]=pos[1];pos[1]=tmp;}
      
	pos[2]-=0.5f*mOffset[2]*(mScale-1)/mScale;
	pos[2]=-abs(-pos[2]);
	pos[2]+=0.5f*mOffset[2]*(mScale-1)/mScale;
      
	pos.Mul(mRotate2Matrix);

	pos[0]=mScale*pos[0]-mPOffset[0];
	pos[1]=mScale*pos[1]-mPOffset[1];
	pos[2]=mScale*pos[2];

	scale *= mScale;
}

void DFractalMandelBox::Read(TiXmlElement* element)
{
	Convert::StrToVec3(element->Attribute("rotate1"), mRotate1);
	Convert::StrToFloat(element->Attribute("scale"), mScale);
	mRotate1Matrix.MakeFromRPY(mRotate1[0],mRotate1[1],mRotate1[2]);
}

void DFractalMandelBox::Calculate(DVector3& pos, const DVector3& origPos, float& scale)
{
	pos.Mul(mRotate1Matrix);
	float fixedRadius = 1.0;
	float fR2 = fixedRadius * fixedRadius;
	float minRadius = 0.5f;
	float mR2 = minRadius * minRadius;
		
	if (pos[0] > 1)
        pos[0] = 2 - pos[0];
    else if (pos[0] < -1)
        pos[0] = -2 - pos[0];
    if (pos[1] > 1)
        pos[1] = 2 - pos[1];
    else if (pos[1] < -1)
        pos[1] = -2 - pos[1];
    if (pos[2] > 1)
        pos[2] = 2 - pos[2];
    else if (pos[2] < -1)
        pos[2] = -2 - pos[2];

	float r2 = pos.MagnitudeSquared();

	if (r2 < mR2)
	{
		pos *= fR2 / mR2;
		scale*= fR2 / mR2;
	}
	else if (r2 < fR2)
	{
		pos *= fR2 / r2;
		scale*= fR2 / r2;
	}
			
    pos = pos * mScale + origPos;
	scale = scale * mScale + 1.0f;
}

void DFractalMandelBulb::Read(TiXmlElement* element)
{
	Convert::StrToVec3(element->Attribute("rotate1"), mRotate1);
	Convert::StrToFloat(element->Attribute("scale"), mScale);
	mRotate1Matrix.MakeFromRPY(mRotate1[0],mRotate1[1],mRotate1[2]);
}

void DFractalMandelBulb::Calculate(DVector3& pos, const DVector3& origPos, float& scale)
{
	float r = pos.Magnitude();
	// convert to polar coordinates
	float theta = acos(pos[2]/r);
	float phi = atan2(pos[1],pos[0]);
	scale =  pow( r, mScale-1.0f)*mScale*scale + 1.0f;

	// scale and rotate the point
	float zr = pow( r,mScale);
	theta = theta*mScale;
	phi = phi*mScale;

	// convert back to cartesian coordinates
//			z = DVector3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta))*zr + pos;
	pos.mComponent[0] = sinf(theta)*cosf(phi)*zr + origPos.mComponent[0];
	pos.mComponent[1] = sinf(phi)*sinf(theta)*zr + origPos.mComponent[1];
	pos.mComponent[2] = cosf(theta)*zr + origPos.mComponent[2];

	return;
}

void DRenderFractal::InternalRead(TiXmlElement* element)
{
	DRenderDistance::InternalRead(element);

	TiXmlElement* iterationXml = (TiXmlElement*) element->FirstChildElement("ITERATION");
	while (iterationXml)
	{
		int type;
		Convert::StrToInt(iterationXml->Attribute("type"), type);

		DFractalIteration* iteration = NULL;
		switch(type)
		{
		case 0:
			iteration = new DFractalTetra();
			iteration->Read(iterationXml);
			_FractalIterations.push_back(iteration);
			break;
		case 1:
			iteration = new DFractalCuboid();
			iteration->Read(iterationXml);
			_FractalIterations.push_back(iteration);
			break;
		case 2:
			iteration = new DFractalMenger();
			iteration->Read(iterationXml);
			_FractalIterations.push_back(iteration);
			break;
		case 3:
			iteration = new DFractalMandelBox();
			iteration->Read(iterationXml);
			_FractalIterations.push_back(iteration);
			break;
		case 4:
			iteration = new DFractalMandelBulb();
			iteration->Read(iterationXml);
			_FractalIterations.push_back(iteration);
			break;
		}
		iterationXml = (TiXmlElement*) iterationXml->NextSiblingElement("ITERATION");
	}
}

float sphere2(DVector3 pos, DVector3 centre, float radius)
{
	return (pos-centre).Magnitude() - radius;
}

float DRenderFractal::DistanceEstimator(DVector3& pos) const
{
	if (_FractalIterations.size()==0) return 10000000;
	DVector3 modpos = pos;
	float modscale = 1;
	unsigned int fractalIteration = 0;
	for (int i=0; i<15; i++)
	{
		_FractalIterations[fractalIteration]->Calculate(modpos, pos, modscale);

		fractalIteration++;
		if (fractalIteration>=_FractalIterations.size())
			fractalIteration = 0;
	}
	return (modpos.Magnitude() - 1) / abs(modscale);
//	return max(sphere2(pos, DVector3(0, 0, 0.3f), 0.7f), - sphere2(pos, DVector3(0,0,-0.5f), 0.5f));
}

DColour DRenderFractal::DEColour(const DVector3& pos) const
{
	if (_FractalIterations.size()==0) return DColour(1,0,0);
	DVector3 modpos = pos;
	DVector3 trap = DVector3(1000000,1000000,1000000);
	float modscale = 1;
	unsigned int fractalIteration = 0;
	for (int i=0; i<15; i++)
	{
		_FractalIterations[fractalIteration]->Calculate(modpos, pos, modscale);

		trap.SetMin(trap, modpos.GetAbs());

		fractalIteration++;
		if (fractalIteration>=_FractalIterations.size())
			fractalIteration = 0;
	}
	return trap;
}