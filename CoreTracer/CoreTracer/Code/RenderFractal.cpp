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
/*	  float x = pos[0]; float x2 = x*x; float x4 = x2*x2;
  float y = pos[2]; float y2 = y*y; float y4 = y2*y2;
  float z = pos[1]; float z2 = z*z; float z4 = z2*z2;
	scale =  pow( pos.Magnitude(), 8-1.0f)*8*8 + 1.0f;

  float k3 = x2 + z2;
  float k2 = 1/sqrtf( k3*k3*k3*k3*k3*k3*k3 );
  float k1 = x4 + y4 + z4 - 6.0*y2*z2 - 6.0*x2*y2 + 2.0*z2*x2;
  float k4 = x2 - y2 + z2;

  pos[0] =  64.0*x*y*z*(x2-z2)*k4*(x4-6.0*x2*z2+z4)*k1*k2;
  pos[2] = -16.0*y2*k3*k4*k4 + k1*k1;
  pos[1] = -8.0*y*k4*(x4*x4 - 28.0*x4*x2*z2 + 70.0*x4*z4 - 28.0*x2*z2*z4 + z4*z4)*k1*k2;
  pos += origPos;
  */
	float r = pos.Magnitude();
	float theta = acos(pos[1]/r);
	float phi = atan2(pos[2],pos[0]);
	scale =  pow( r, mScale-1.0f)*mScale*scale + 1.0f;

	// scale and rotate the point
	float zr = pow( r,mScale);
	theta = theta*mScale;
	phi = phi*mScale;

	// convert back to cartesian coordinates
	pos.mComponent[0] = sinf(theta)*cosf(phi)*zr + origPos.mComponent[0];
	pos.mComponent[2] = sinf(phi)*sinf(theta)*zr + origPos.mComponent[2];
	pos.mComponent[1] = cosf(theta)*zr + origPos.mComponent[1];

	return;
}

void DRenderFractal::InternalRead(TiXmlElement* element)
{
	mFractalIterations = mColourIterations = 15;
	DRenderDistance::InternalRead(element);
	Convert::StrToInt(element->Attribute("iterationCount"), mFractalIterations);
	Convert::StrToInt(element->Attribute("colourIterationCount"), mColourIterations);
	Convert::StrToInt(element->Attribute("DEMode"), mDEMode);

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

float DRenderFractal::EscapeLength(DVector3& pos, DVector3& modpos, float& modscale, int& iterations) const
{
	modpos = pos;
	modscale = 1;
	float r=0;

	unsigned int fractalIteration = 0;
	int i;
	for (i=0; i<mFractalIterations; i++)
	{
		r = modpos.Magnitude();
		if ((r>100 && iterations==0)
			|| (i>0 && i==iterations)) break;

		_FractalIterations[fractalIteration]->Calculate(modpos, pos, modscale);

		fractalIteration++;
		if (fractalIteration>=_FractalIterations.size())
			fractalIteration = 0;
	}

	if (iterations==0) iterations = i;

	return r;
}

float DRenderFractal::DistanceEstimator(DVector3& pos) const
{
	if (_FractalIterations.size()==0) return 10000000;

	DVector3 modpos = pos;
	float modscale = 1;
	float r;
	int iterations=0;

	r = EscapeLength(pos, modpos, modscale, iterations);

	if (r<0) return 0.0;

	if (mDEMode==3)
	{
		// Makin/Buddhi method
		const float delta = 0.001f;
		DVector3 gradient((EscapeLength(pos+DVector3(delta, 0, 0), modpos, modscale, iterations) - r) / delta,
			(EscapeLength(pos+DVector3(0, delta, 0), modpos, modscale, iterations) - r) / delta,
			(EscapeLength(pos+DVector3(0, 0, delta), modpos, modscale, iterations) - r) / delta);

		float gradlength = gradient.Magnitude();
		if (gradlength>0)
			return 0.5f*log(r)*r/gradient.Magnitude();
		else
			return 0.001f;
	}

	if (mDEMode==0)
		return 0.5f*log(r)*r/modscale;  // bulb method
	if (mDEMode==1)
		return (r - 1) / abs(modscale); // KIFS method
	if (mDEMode==2)
		return _FractalIterations[iterations%_FractalIterations.size()]->Estimate(r, modscale); // last iteration method

	return 0.0;
//	return max(sphere2(pos, DVector3(0, 0, 0.3f), 0.7f), - sphere2(pos, DVector3(0,0,-0.5f), 0.5f));
}

DColour DRenderFractal::DEColour(const DVector3& pos) const
{
	if (_FractalIterations.size()==0) return DColour(1,0,0);
	DVector3 modpos = pos;
	DVector3 trap = DVector3(1000000,1000000,1000000);
	float modscale = 1;
	float r;
	unsigned int fractalIteration = 0;
	for (int i=0; i<mColourIterations; i++)
	{
		r = modpos.Magnitude();
		if (r>100) break;

		_FractalIterations[fractalIteration]->Calculate(modpos, pos, modscale);

		trap.SetMin(trap, modpos.GetAbs());

		fractalIteration++;
		if (fractalIteration>=_FractalIterations.size())
			fractalIteration = 0;
	}
	return trap;
}