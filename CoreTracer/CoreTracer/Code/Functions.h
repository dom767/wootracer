#ifndef DISTFUNCTIONS_H
#define DISTFUNCTIONS_H

#include "FuncCore.h"
#include "FuncFactory.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "StaticCentres.h"
#include "TextureCache.h"
//#include <string>
#include <sstream>

BEGIN_FUNC(DDistSphere, "sphere");
	DDistSphere()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mCentre", Vec));
		mParam.push_back(new DFuncParam("mRadius", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		return (mParam[0]->EvaluateVec(state)-mParam[1]->EvaluateVec(state)).Magnitude() - mParam[2]->Evaluate(state);
	}
END_FUNC

BEGIN_FUNC(DDistCapsule, "capsule");
	DDistCapsule()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mLength", Float));
		mParam.push_back(new DFuncParam("mRadius", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		DVector3 pos = mParam[0]->EvaluateVec(state);
		float length = mParam[1]->Evaluate(state);
		float rad = mParam[2]->Evaluate(state);
		DVector3 pa = pos - DVector3(0, -length+rad, 0);
		DVector3 ba = DVector3(0, length*2 - rad*2, 0);
		float h = clamp(pa.Dot(ba)/ba.Dot(ba), 0.0f, 1.0f);
		return (pa - ba*h).Magnitude() - rad;
	}
END_FUNC

BEGIN_FUNC(DDistCylinder, "cylinder");
	DDistCylinder()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mLength", Float));
		mParam.push_back(new DFuncParam("mRadius", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		DVector3 pos = mParam[0]->EvaluateVec(state);
		float length = mParam[1]->Evaluate(state);
		float rad = mParam[2]->Evaluate(state);

		float px = (pos[0] - rad);
		float pz = (pos[2] - rad);
		return sqrtf(px*px+pz*pz) - length;
	}
END_FUNC

BEGIN_FUNC(DDistMin, "min");
	DDistMin()
	{
		mParam.push_back(new DFuncParam("mFunc1", Float));
		mParam.push_back(new DFuncParam("mFunc2", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		return min(mParam[0]->Evaluate(state), mParam[1]->Evaluate(state));
	}
END_FUNC

BEGIN_VECFUNC(DDistVecMin, "min");
	DDistVecMin()
	{
		mParam.push_back(new DFuncParam("mFunc1", Vec));
		mParam.push_back(new DFuncParam("mFunc2", Vec));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 arg1 = mParam[0]->EvaluateVec(state);
		DVector3 arg2 = mParam[1]->EvaluateVec(state);
		return DVector3(min(arg1[0], arg2[0]),
			min(arg1[1], arg2[1]),
			min(arg1[2], arg2[2]));
	}
END_FUNC

BEGIN_FUNC(DDistSMin, "smin");
	DDistSMin()
	{
		mParam.push_back(new DFuncParam("mFunc1", Float));
		mParam.push_back(new DFuncParam("mFunc2", Float));
		mParam.push_back(new DFuncParam("mK", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		float mK = mParam[2]->Evaluate(state);
		float res = exp( -mK*mParam[0]->Evaluate(state) ) + exp( -mK*mParam[1]->Evaluate(state) );
		return -log( res )/(mK);
	}
END_FUNC

BEGIN_FUNC(DDistSMin3, "smin3");
	DDistSMin3()
	{
		mParam.push_back(new DFuncParam("mFunc1", Float));
		mParam.push_back(new DFuncParam("mFunc2", Float));
		mParam.push_back(new DFuncParam("mFunc3", Float));
		mParam.push_back(new DFuncParam("mK", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		float mK = mParam[3]->Evaluate(state);
		float res = exp( -mK*mParam[0]->Evaluate(state) )
			+ exp( -mK*mParam[1]->Evaluate(state) )
			+ exp( -mK*mParam[2]->Evaluate(state) );
		return -log( res )/(mK);
	}
END_FUNC

BEGIN_FUNC(DDistMax, "max");
	DDistMax()
	{
		mParam.push_back(new DFuncParam("mFunc1", Float));
		mParam.push_back(new DFuncParam("mFunc2", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		return max(mParam[0]->Evaluate(state), mParam[1]->Evaluate(state));
	}
END_FUNC

BEGIN_FUNC(DDistNeg, "neg");
	DDistNeg()
	{
		mParam.push_back(new DFuncParam("mVal", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		return -mParam[0]->Evaluate(state);
	}
END_FUNC

BEGIN_FUNC(DDistMag, "mag");
	DDistMag()
	{
		mParam.push_back(new DFuncParam("mVec", Vec));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		return mParam[0]->EvaluateVec(state).Magnitude();
	}
END_FUNC

BEGIN_VECFUNC(DDistNormalise, "normalise");
	DDistNormalise()
	{
		mParam.push_back(new DFuncParam("mVec", Vec));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 vec = mParam[0]->EvaluateVec(state);
		vec.Normalise();
		return vec;
	}
END_FUNC

BEGIN_FUNC(DDistSub, "sub");
	DDistSub()
	{
		mParam.push_back(new DFuncParam("mFunc1", Float));
		mParam.push_back(new DFuncParam("mFunc2", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		return mParam[0]->Evaluate(state) - mParam[1]->Evaluate(state);
	}
END_FUNC

BEGIN_FUNC(DDistFloatVar, "floatVar");
	DDistFloatVar()
	{
		mConstant = false;
	}

	virtual float Evaluate(DFunctionState& state)
	{
		return DFuncFactory::Get().GetFloatValue(state, mVariable);
	}

	void Set(DFunctionState& state, float val)
	{
		DFuncFactory::Get().SetFloatValue(state, mVariable, val);
	};

	void SetVariable(DVariable* variable) {mVariable = variable;}

	DVariable* mVariable;
END_FUNC

BEGIN_FUNC(DLerpF, "lerp");
	DLerpF()
	{
		mParam.push_back(new DFuncParam("mFloat1", Float));
		mParam.push_back(new DFuncParam("mFloat2", Float));
		mParam.push_back(new DFuncParam("mAmount", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		float mFloat1 = mParam[0]->Evaluate(state);
		float mFloat2 = mParam[1]->Evaluate(state);
		float mAmount = clamp(mParam[2]->Evaluate(state),0,1);
		float invAmount = 1-mAmount;

		return mFloat1*invAmount + mFloat2*mAmount;
	}
END_FUNC

BEGIN_VECFUNC(DDistVectorVar, "vecVar");
	DDistVectorVar()
	{
		mConstant = false;
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		return DFuncFactory::Get().GetVectorValue(state, mVariable);
	}

	void Set(DFunctionState& state, DVector3& val)
	{
		DFuncFactory::Get().SetVectorValue(state, mVariable, val);
	};

	void SetVariable(DVariable* variable) {mVariable = variable;}

	DVariable* mVariable;
END_FUNC

BEGIN_FUNC(DDistFloat, "float");
	DDistFloat()
	{
		mParam.push_back(new DFuncParam("mRawFloat", RawFloat));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		return mFloat;
	}

	void SetFloat(float val) {mFloat = val;}

	float mFloat;
END_FUNC

BEGIN_NULLFUNC(DDistCodeblock, "codeblock");
	DDistCodeblock()
	{
	}

	virtual void Evaluate(DFunctionState& state)
	{
		mProgram.Run(state);
	}

	void Parse(std::string& arg)
	{
		mProgram.Create(arg);
	}

	DProgram mProgram;
END_FUNC

BEGIN_NULLFUNC(DDistRepeat, "repeat");
	DDistRepeat()
	{
		mParam.push_back(new DFuncParam("mRepeats", Float));
		mParam.push_back(new DFuncParam("mCodeblock", Null));
	}

	virtual void Evaluate(DFunctionState& state)
	{
		for (int i=0; i<mParam[0]->Evaluate(state); i++)
		{
			mParam[1]->EvaluateNull(state);
		}
	}
END_FUNC

BEGIN_FUNC(DDistMandelBox, "mandelbox");
	DDistMandelBox()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mIterations", Float));
		mParam.push_back(new DFuncParam("mScale", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		DVector3 mpos = mParam[0]->EvaluateVec(state);
		int mIterations = (int)(mParam[1]->Evaluate(state));
		float mScale = mParam[2]->Evaluate(state);

        DVector3 c = mpos;
        float dr = 1.0f;
		float fixedRadius = 1.0;
		float fR2 = fixedRadius * fixedRadius;
		float minRadius = 0.5f;
		float mR2 = minRadius * minRadius;

		for (int n = 0; n < mIterations; n++)
        {
            if (mpos[0] > 1)
                mpos[0] = 2 - mpos[0];
            else if (mpos[0] < -1)
                mpos[0] = -2 - mpos[0];
            if (mpos[1] > 1)
                mpos[1] = 2 - mpos[1];
            else if (mpos[1] < -1)
                mpos[1] = -2 - mpos[1];
            if (mpos[2] > 1)
                mpos[2] = 2 - mpos[2];
            else if (mpos[2] < -1)
                mpos[2] = -2 - mpos[2];
			/*
            float r2 = mpos.MagnitudeSquared();

            if (r2 < 1)
            {
                if (r2 < 0.25f) // 0.5 squared
                {
                    mpos *= 4;
                    dr *= 4;
                }
                else
                {
                    mpos /= r2;
                    dr /= r2;
                }
            }*/
/*			
            float radiusSquared = mpos.MagnitudeSquared();
			if (radiusSquared<1)
			{
				float radius = sqrtf(radiusSquared);
				mpos = mpos * (2-radius) / radius;
			}*/

			float r2 = mpos.MagnitudeSquared();

			if (r2 < mR2)
			{
				mpos *= fR2 / mR2;
				dr*= fR2 / mR2;
			}
			else if (r2 < fR2)
			{
				mpos *= fR2 / r2;
				dr*= fR2 / r2;
			}
			
            mpos = mpos * mScale + c;
            dr = dr * mScale + 1.0f;
        }
        return mpos.Magnitude() / abs(dr);
	}
END_FUNC

BEGIN_VECFUNC(DDistMandelBoxStep, "mandelboxstep");
	DDistMandelBoxStep()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mNewPos", Vec));
		mParam.push_back(new DFuncParam("mScale", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 mOldPos = mParam[0]->EvaluateVec(state);
		DVector3 mpos = mParam[1]->EvaluateVec(state);
		float mScale = mParam[2]->Evaluate(state);
		float fixedRadius = 1.0;
		float fR2 = fixedRadius * fixedRadius;
		float minRadius = 0.5f;
		float mR2 = minRadius * minRadius;

        DVector3 c = mOldPos;
        float dr = 1.0f;

		if (mpos[0] > 1)
            mpos[0] = 2 - mpos[0];
        else if (mpos[0] < -1)
            mpos[0] = -2 - mpos[0];
        if (mpos[1] > 1)
            mpos[1] = 2 - mpos[1];
        else if (mpos[1] < -1)
            mpos[1] = -2 - mpos[1];
        if (mpos[2] > 1)
            mpos[2] = 2 - mpos[2];
        else if (mpos[2] < -1)
            mpos[2] = -2 - mpos[2];

		float r2 = mpos.MagnitudeSquared();

		if (r2 < mR2)
		{
			mpos *= fR2 / mR2;
			dr*= fR2 / mR2;
		}
		else if (r2 < fR2)
		{
			mpos *= fR2 / r2;
			dr*= fR2 / r2;
		}

			
        mpos = mpos * mScale + c;
		
		return mpos;
	}
END_FUNC

BEGIN_FUNC(DDistMandelBoxStepScale, "mandelboxstepscale");
	DDistMandelBoxStepScale()
	{
		mParam.push_back(new DFuncParam("mOldScale", Float));
		mParam.push_back(new DFuncParam("mNewPos", Vec));
		mParam.push_back(new DFuncParam("mScale", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		float dr = mParam[0]->Evaluate(state);
		DVector3 mpos = mParam[1]->EvaluateVec(state);
		float mScale = mParam[2]->Evaluate(state);
		float fixedRadius = 1.0;
		float fR2 = fixedRadius * fixedRadius;
		float minRadius = 0.5f;
		float mR2 = minRadius * minRadius;

		if (mpos[0] > 1)
            mpos[0] = 2 - mpos[0];
        else if (mpos[0] < -1)
            mpos[0] = -2 - mpos[0];
        if (mpos[1] > 1)
            mpos[1] = 2 - mpos[1];
        else if (mpos[1] < -1)
            mpos[1] = -2 - mpos[1];
        if (mpos[2] > 1)
            mpos[2] = 2 - mpos[2];
        else if (mpos[2] < -1)
            mpos[2] = -2 - mpos[2];

		float r2 = mpos.MagnitudeSquared();

		if (r2 < mR2)
		{
			dr*= fR2 / mR2;
		}
		else if (r2 < fR2)
		{
			dr*= fR2 / r2;
		}

        dr = dr * mScale + 1.0f;
		
		return dr;
	}
END_FUNC

BEGIN_FUNC(DDistMandelBulb, "mandelbulb");
	DDistMandelBulb()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mIterations", Float));
		mParam.push_back(new DFuncParam("mBailout", Float));
		mParam.push_back(new DFuncParam("mPower", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		int Iterations = int(mParam[1]->Evaluate(state)+0.5f);
		float Bailout = mParam[2]->Evaluate(state);
		float Power = mParam[3]->Evaluate(state);

		DVector3 pos = mParam[0]->EvaluateVec(state);
		DVector3 z = pos;
		float dr = 1.0;
		float r = 0.0;
		float theta, phi, zr;
		for (int i = 0; i < Iterations ; i++) {
			r = z.Magnitude();
			if (r>Bailout) break;

			// convert to polar coordinates
			theta = acos(z[2]/r);
			phi = atan2(z[1],z[0]);
			dr =  pow( r, Power-1.0f)*Power*dr + 1.0f;

			// scale and rotate the point
			zr = pow( r,Power);
			theta = theta*Power;
			phi = phi*Power;

			// convert back to cartesian coordinates
//			z = DVector3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta))*zr + pos;
			z.mComponent[0] = sinf(theta)*cosf(phi)*zr + pos.mComponent[0];
			z.mComponent[1] = sinf(phi)*sinf(theta)*zr + pos.mComponent[1];
			z.mComponent[2] = cosf(theta)*zr + pos.mComponent[2];
		}
		return 0.5f*log(r)*r/dr;
	}
END_FUNC

BEGIN_FUNC(DDistSignedBox, "box");
	DDistSignedBox()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mDimensions", Vec));
		mParam.push_back(new DFuncParam("mOffset", Vec));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		DVector3 worldPos = mParam[0]->EvaluateVec(state) - mParam[2]->EvaluateVec(state);
		DVector3 d = DVector3(abs(worldPos[0]), abs(worldPos[1]), abs(worldPos[2])) - mParam[1]->EvaluateVec(state);
		return (float)min(max(d[0],max(d[1],d[2])),0.0) + DVector3(max(d[0],0), max(d[1], 0), max(d[2], 0)).Magnitude();
	}
END_FUNC

BEGIN_FUNC(DDistTetra, "tetra");
	DDistTetra()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
	}

	float DistanceFromPlane(DVector3 pos)
	{
	}

	virtual float Evaluate(DFunctionState& state)
	{
		DVector3 pos = mParam[0]->EvaluateVec(state);
		return pos[0];
	}
END_FUNC

BEGIN_FUNC(DDistSignedTorus, "torus");
	DDistSignedTorus()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mR1", Float));
		mParam.push_back(new DFuncParam("mR2", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		float r2 = mParam[2]->Evaluate(state);
		float x = sqrtf(rPos[0]*rPos[0] + rPos[1]*rPos[1]) - mParam[1]->Evaluate(state);
		float z = rPos[2];
		return sqrtf(x*x + z*z) - r2;
	}
END_FUNC

BEGIN_FUNC(DDistGetX, "getx");
	DDistGetX()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		return rPos[0];
	}
END_FUNC

BEGIN_FUNC(DDistGetY, "gety");
	DDistGetY()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		return rPos[1];
	}
END_FUNC

BEGIN_FUNC(DDistGetZ, "getz");
	DDistGetZ()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		return rPos[2];
	}
END_FUNC

static float smod(float arg, float arg2)
{
	float modf = fmodf(arg, arg2);
	if (modf<0) modf += arg2;
	return modf;
}

BEGIN_FUNC(DDistMul, "mul");
	DDistMul()
	{
		mParam.push_back(new DFuncParam("mArg", Float));
		mParam.push_back(new DFuncParam("mMul", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		float rPos = mParam[0]->Evaluate(state);
		float mul = mParam[1]->Evaluate(state);
		return rPos * mul;
	}
END_FUNC

BEGIN_FUNC(DDistKaleido, "kaleido");
	DDistKaleido()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRot1", Vec));
		mParam.push_back(new DFuncParam("mRot2", Vec));
		mParam.push_back(new DFuncParam("mOffset", Vec));
		mParam.push_back(new DFuncParam("mIterations", Float));
		mParam.push_back(new DFuncParam("mScale", Float));
		mRot1 = mRot2 = DVector3(0,0,0);
		matRot1.MakeIdentity();
		matRot2.MakeIdentity();
	}

	DMatrix4 rotate1(const DVector3& rot)
	{
		DMatrix4 mat;
		mat.MakeFromRPY(rot[0],rot[1],rot[2]);
		return mat;
	}

	DVector3 mRot1;
	DVector3 mRot2;
	DMatrix4 matRot1;
	DMatrix4 matRot2;

	virtual float Evaluate(DFunctionState& state)
	{
		DVector3 pos = mParam[0]->EvaluateVec(state);
		DVector3 rot1 = mParam[1]->EvaluateVec(state);
		DVector3 rot2 = mParam[2]->EvaluateVec(state);
		DVector3 offset = mParam[3]->EvaluateVec(state);
		int iterations = int(0.5f + mParam[4]->Evaluate(state));
		float scale = mParam[5]->Evaluate(state);

		if (mRot1[0]!=rot1[0] || mRot1[1]!=rot1[1] || mRot1[2]!=rot1[2])
		{
			matRot1 = rotate1(rot1);
			mRot1 = rot1;
		}
		if (mRot2[0]!=rot2[0] || mRot2[1]!=rot2[1] || mRot2[2]!=rot2[2])
		{
			matRot2 = rotate1(rot2);
			mRot2 = rot2;
		}

		float rScale = 1;
		DVector3 pOffset = offset*(scale-1);
		float tmp;
		for(int i=0;i<iterations;i++)
		{
			pos.Mul(matRot1);

			if(pos[0]+pos[1]<0){tmp=-pos[1];pos[1]=-pos[0];pos[0]=tmp;}
			if(pos[0]+pos[2]<0){tmp=-pos[2];pos[2]=-pos[0];pos[0]=tmp;}
			if(pos[1]+pos[2]<0){tmp=-pos[2];pos[2]=-pos[1];pos[1]=tmp;}
      
			pos.Mul(matRot2);

			pos = pos*scale - pOffset;
			rScale *= scale;
		}	

		return (pos.Magnitude()-2)/rScale;//*powf(scale,-float(i));//the estimated distance
	}
END_FUNC

BEGIN_VECFUNC(DDistKalTetraStep, "kaltetrastep");
	DDistKalTetraStep()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRot1", Vec));
		mParam.push_back(new DFuncParam("mRot2", Vec));
		mParam.push_back(new DFuncParam("mOffset", Vec));
		mParam.push_back(new DFuncParam("mScale", Float));
		matRot1.MakeIdentity();
		matRot2.MakeIdentity();
		mInitialised = false;
	}

	DMatrix4 rotate1(const DVector3& rot)
	{
		DMatrix4 mat;
		mat.MakeFromRPY(rot[0],rot[1],rot[2]);
		return mat;
	}

	DMatrix4 matRot1;
	DMatrix4 matRot2;
	DVector3 mOffset;
	DVector3 mPOffset;
	float mScale;
	bool mInitialised;

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 pos = mParam[0]->EvaluateVec(state);

		if (!mInitialised)
		{
			DVector3 rot1 = mParam[1]->EvaluateVec(state);
			matRot1 = rotate1(rot1);
			DVector3 rot2 = mParam[2]->EvaluateVec(state);
			matRot2 = rotate1(rot2);
			mOffset = mParam[3]->EvaluateVec(state);
			mScale = mParam[4]->Evaluate(state);
			mPOffset = mOffset*(mScale-1);
			mInitialised = true;
		}

		float tmp;

		pos.Mul(matRot1);

		if(pos[0]+pos[1]<0){tmp=-pos[1];pos[1]=-pos[0];pos[0]=tmp;}
		if(pos[0]+pos[2]<0){tmp=-pos[2];pos[2]=-pos[0];pos[0]=tmp;}
		if(pos[1]+pos[2]<0){tmp=-pos[2];pos[2]=-pos[1];pos[1]=tmp;}
      
		pos.Mul(matRot2);

		pos = pos*mScale - mPOffset;

		return pos;
	}
END_FUNC

BEGIN_VECFUNC(DDistKalCubeStep, "kalcubestep");
	DDistKalCubeStep()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRot1", Vec));
		mParam.push_back(new DFuncParam("mRot2", Vec));
		mParam.push_back(new DFuncParam("mOffset", Vec));
		mParam.push_back(new DFuncParam("mScale", Float));
		matRot1.MakeIdentity();
		matRot2.MakeIdentity();
		mInitialised = false;
	}

	DMatrix4 rotate1(const DVector3& rot)
	{
		DMatrix4 mat;
		mat.MakeFromRPY(rot[0],rot[1],rot[2]);
		return mat;
	}

	DMatrix4 matRot1;
	DMatrix4 matRot2;
	DVector3 mOffset;
	DVector3 mPOffset;
	float mScale;
	bool mInitialised;

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 pos = mParam[0]->EvaluateVec(state);

		if (!mInitialised)
		{
			DVector3 rot1 = mParam[1]->EvaluateVec(state);
			matRot1 = rotate1(rot1);
			DVector3 rot2 = mParam[2]->EvaluateVec(state);
			matRot2 = rotate1(rot2);
			mOffset = mParam[3]->EvaluateVec(state);
			mScale = mParam[4]->Evaluate(state);
			mPOffset = mOffset*(mScale-1);
			mInitialised = true;
		}

		pos.Mul(matRot1);

		pos[0] = abs(pos[0]);
		pos[1] = abs(pos[1]);
		pos[2] = abs(pos[2]);
      
		pos.Mul(matRot2);

		pos = pos*mScale - mPOffset;

		return pos;
	}
END_FUNC

BEGIN_VECFUNC(DDistKalMengerStep, "kalmengerstep");
	DDistKalMengerStep()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRot1", Vec));
		mParam.push_back(new DFuncParam("mRot2", Vec));
		mParam.push_back(new DFuncParam("mOffset", Vec));
		mParam.push_back(new DFuncParam("mScale", Float));
		matRot1.MakeIdentity();
		matRot2.MakeIdentity();
		mInitialised = false;
	}

	DMatrix4 rotate1(const DVector3& rot)
	{
		DMatrix4 mat;
		mat.MakeFromRPY(rot[0],rot[1],rot[2]);
		return mat;
	}

	DMatrix4 matRot1;
	DMatrix4 matRot2;
	DVector3 mOffset;
	DVector3 mPOffset;
	float mScale;
	bool mInitialised;

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 pos = mParam[0]->EvaluateVec(state);

		if (!mInitialised)
		{
			DVector3 rot1 = mParam[1]->EvaluateVec(state);
			matRot1 = rotate1(rot1);
			DVector3 rot2 = mParam[2]->EvaluateVec(state);
			matRot2 = rotate1(rot2);
			mOffset = mParam[3]->EvaluateVec(state);
			mScale = mParam[4]->Evaluate(state);
			mPOffset = mOffset*(mScale-1);
			mInitialised = true;
		}

		pos.Mul(matRot1);

		float tmp;

		pos.SetAbs();
		if(pos[0]-pos[1]<0){tmp=pos[1];pos[1]=pos[0];pos[0]=tmp;}
		if(pos[0]-pos[2]<0){tmp=pos[2];pos[2]=pos[0];pos[0]=tmp;}
		if(pos[1]-pos[2]<0){tmp=pos[2];pos[2]=pos[1];pos[1]=tmp;}
      
		pos[2]-=0.5f*mOffset[2]*(mScale-1)/mScale;
		pos[2]=-abs(-pos[2]);
		pos[2]+=0.5f*mOffset[2]*(mScale-1)/mScale;
      
		pos.Mul(matRot2);

		pos[0]=mScale*pos[0]-mOffset[0]*(mScale-1);
		pos[1]=mScale*pos[1]-mOffset[1]*(mScale-1);
		pos[2]=mScale*pos[2];

		return pos;
	}
END_FUNC

BEGIN_VECFUNC(DDistKaleidoCol, "kaleidocol");
	DDistKaleidoCol()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRot1", Vec));
		mParam.push_back(new DFuncParam("mRot2", Vec));
		mParam.push_back(new DFuncParam("mOffset", Vec));
		mParam.push_back(new DFuncParam("mIterations", Float));
		mParam.push_back(new DFuncParam("mScale", Float));
		mRot1 = mRot2 = DVector3(0,0,0);
		matRot1.MakeIdentity();
		matRot2.MakeIdentity();
	}

	DMatrix4 rotate1(const DVector3& rot)
	{
		DMatrix4 mat;
		mat.MakeFromRPY(rot[0],rot[1],rot[2]);
		return mat;
	}

	DVector3 mRot1;
	DVector3 mRot2;
	DMatrix4 matRot1;
	DMatrix4 matRot2;

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 pos = mParam[0]->EvaluateVec(state);
		DVector3 rot1 = mParam[1]->EvaluateVec(state);
		DVector3 rot2 = mParam[2]->EvaluateVec(state);
		DVector3 offset = mParam[3]->EvaluateVec(state);
		int iterations = int(0.5f + mParam[4]->Evaluate(state));
		float scale = mParam[5]->Evaluate(state);

		if (mRot1[0]!=rot1[0] || mRot1[1]!=rot1[1] || mRot1[2]!=rot1[2])
		{
			matRot1 = rotate1(rot1);
			mRot1 = rot1;
		}
		if (mRot2[0]!=rot2[0] || mRot2[1]!=rot2[1] || mRot2[2]!=rot2[2])
		{
			matRot2 = rotate1(rot2);
			mRot2 = rot2;
		}

		DVector3 pOffset = offset*(scale-1);
		float tmp;
		float acc[3];
		acc[0] = acc[1] = acc[2] = 0;
		int i;
		for(i=0;i<iterations;i++)
		{
			pos.Mul(matRot1);

			if(pos[0]+pos[1]<0){acc[0]++;tmp=-pos[1];pos[1]=-pos[0];pos[0]=tmp;}
			if(pos[0]+pos[2]<0){acc[1]++;tmp=-pos[2];pos[2]=-pos[0];pos[0]=tmp;}
			if(pos[1]+pos[2]<0){acc[2]++;tmp=-pos[2];pos[2]=-pos[1];pos[1]=tmp;}
      
			pos.Mul(matRot2);

			pos = pos*scale - pOffset;
		}	

		DVector3 ret;
		acc[0] = fabsf(acc[0]-float(i)*0.5f)/float(i);
		acc[1] = fabsf(acc[1]-float(i)*0.5f)/float(i);
		acc[2] = fabsf(acc[2]-float(i)*0.5f)/float(i);

		ret[0] = ret[1] = ret[2] = (acc[0] + acc[1] + acc[2]) / 1.5f;
		return ret;
	}
END_FUNC

BEGIN_VECFUNC(DDistKaleidoTrap, "kaleidotrap");
	DDistKaleidoTrap()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRot1", Vec));
		mParam.push_back(new DFuncParam("mRot2", Vec));
		mParam.push_back(new DFuncParam("mOffset", Vec));
		mParam.push_back(new DFuncParam("mIterations", Float));
		mParam.push_back(new DFuncParam("mScale", Float));
		mParam.push_back(new DFuncParam("mTrap", Float));
		mRot1 = mRot2 = DVector3(0,0,0);
		matRot1.MakeIdentity();
		matRot2.MakeIdentity();
	}

	DMatrix4 rotate1(const DVector3& rot)
	{
		DMatrix4 mat;
		mat.MakeFromRPY(rot[0],rot[1],rot[2]);
		return mat;
	}

	DVector3 mRot1;
	DVector3 mRot2;
	DMatrix4 matRot1;
	DMatrix4 matRot2;

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 pos = mParam[0]->EvaluateVec(state);
		DVector3 rot1 = mParam[1]->EvaluateVec(state);
		DVector3 rot2 = mParam[2]->EvaluateVec(state);
		DVector3 offset = mParam[3]->EvaluateVec(state);
		int iterations = int(0.5f + mParam[4]->Evaluate(state));
		float scale = mParam[5]->Evaluate(state);

		if (mRot1[0]!=rot1[0] || mRot1[1]!=rot1[1] || mRot1[2]!=rot1[2])
		{
			matRot1 = rotate1(rot1);
			mRot1 = rot1;
		}
		if (mRot2[0]!=rot2[0] || mRot2[1]!=rot2[1] || mRot2[2]!=rot2[2])
		{
			matRot2 = rotate1(rot2);
			mRot2 = rot2;
		}

		DVector3 pOffset = offset*(scale-1);
		float trap = 10000000;
		float tmp;
		for(int i=0; i<iterations; i++)
		{
			pos.Mul(matRot1);

			if(pos[0]+pos[1]<0){tmp=-pos[1];pos[1]=-pos[0];pos[0]=tmp;}
			if(pos[0]+pos[2]<0){tmp=-pos[2];pos[2]=-pos[0];pos[0]=tmp;}
			if(pos[1]+pos[2]<0){tmp=-pos[2];pos[2]=-pos[1];pos[1]=tmp;}
      
			pos.Mul(matRot2);

			pos = pos*scale - pOffset;

			state.mTrapPosition = pos;
			trap = min(trap, mParam[6]->Evaluate(state));
		}

		DVector3 ret;
		ret[0] = ret[1] = ret[2] = trap;
		return ret;
	}
END_FUNC

BEGIN_FUNC(DDistKaleidoDETrap, "kaleidodetrap");
	DDistKaleidoDETrap()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRot1", Vec));
		mParam.push_back(new DFuncParam("mRot2", Vec));
		mParam.push_back(new DFuncParam("mOffset", Vec));
		mParam.push_back(new DFuncParam("mIterations", Float));
		mParam.push_back(new DFuncParam("mScale", Float));
		mParam.push_back(new DFuncParam("mTrap", Float));
		mRot1 = mRot2 = DVector3(0,0,0);
		matRot1.MakeIdentity();
		matRot2.MakeIdentity();
	}

	DMatrix4 rotate1(const DVector3& rot)
	{
		DMatrix4 mat;
		mat.MakeFromRPY(rot[0],rot[1],rot[2]);
		return mat;
	}

	DVector3 mRot1;
	DVector3 mRot2;
	DMatrix4 matRot1;
	DMatrix4 matRot2;

	virtual float Evaluate(DFunctionState& state)
	{
		DVector3 pos = mParam[0]->EvaluateVec(state);
		DVector3 rot1 = mParam[1]->EvaluateVec(state);
		DVector3 rot2 = mParam[2]->EvaluateVec(state);
		DVector3 offset = mParam[3]->EvaluateVec(state);
		int iterations = int(0.5f + mParam[4]->Evaluate(state));
		float scale = mParam[5]->Evaluate(state);

		if (mRot1[0]!=rot1[0] || mRot1[1]!=rot1[1] || mRot1[2]!=rot1[2])
		{
			matRot1 = rotate1(rot1);
			mRot1 = rot1;
		}
		if (mRot2[0]!=rot2[0] || mRot2[1]!=rot2[1] || mRot2[2]!=rot2[2])
		{
			matRot2 = rotate1(rot2);
			mRot2 = rot2;
		}

		float rScale = 1;
		DVector3 pOffset = offset*(scale-1);
		float trap = 1000000;
		float tmp;
		for(int i=0;i<iterations;i++)
		{
			pos.Mul(matRot1);

			if(pos[0]+pos[1]<0){tmp=-pos[1];pos[1]=-pos[0];pos[0]=tmp;}
			if(pos[0]+pos[2]<0){tmp=-pos[2];pos[2]=-pos[0];pos[0]=tmp;}
			if(pos[1]+pos[2]<0){tmp=-pos[2];pos[2]=-pos[1];pos[1]=tmp;}
      
			pos.Mul(matRot2);

			pos = pos*scale - pOffset;

			rScale *= scale;

			state.mTrapPosition = pos;
			trap = min(trap, mParam[6]->Evaluate(state)/rScale);
		}	
		return trap;
	}
END_FUNC

BEGIN_FUNC(DAdd, "add");
	DAdd()
	{
		mParam.push_back(new DFuncParam("mVal1", Float));
		mParam.push_back(new DFuncParam("mVal2", Float));
	}
	
	virtual float Evaluate(DFunctionState& state)
	{
		float rVal1 = mParam[0]->Evaluate(state);
		float rVal2 = mParam[1]->Evaluate(state);
		return rVal1 + rVal2;
	}
END_FUNC

BEGIN_FUNC(DDistDiv, "div");
	DDistDiv()
	{
		mParam.push_back(new DFuncParam("mVal1", Float));
		mParam.push_back(new DFuncParam("mVal2", Float));
	}
	
	virtual float Evaluate(DFunctionState& state)
	{
		float rVal1 = mParam[0]->Evaluate(state);
		float rVal2 = mParam[1]->Evaluate(state);
		return rVal1 / rVal2;
	}
END_FUNC

BEGIN_FUNC(DMod, "mod");
	DMod()
	{
		mParam.push_back(new DFuncParam("mVal1", Float));
		mParam.push_back(new DFuncParam("mModAmt", Float));
	}
	
	virtual float Evaluate(DFunctionState& state)
	{
		float val1 = mParam[0]->Evaluate(state);
		float modAmt = mParam[1]->Evaluate(state);
		float ret = fmod(val1, modAmt);
		if (ret<0) ret += modAmt;
		return ret;
	}
END_FUNC

BEGIN_FUNC(DAbs, "abs");
	DAbs()
	{
		mParam.push_back(new DFuncParam("mVal1", Float));
	}
	
	virtual float Evaluate(DFunctionState& state)
	{
		float val1 = mParam[0]->Evaluate(state);
		return fabsf(val1);
	}
END_FUNC

BEGIN_FUNC(DSqrt, "sqrt");
	DSqrt()
	{
		mParam.push_back(new DFuncParam("mVal1", Float));
	}
	
	virtual float Evaluate(DFunctionState& state)
	{
		float val1 = mParam[0]->Evaluate(state);
		return sqrt(val1);
	}
END_FUNC

BEGIN_FUNC(DCos, "cos");
	DCos()
	{
		mParam.push_back(new DFuncParam("mVal1", Float));
	}
	
	virtual float Evaluate(DFunctionState& state)
	{
		float val1 = mParam[0]->Evaluate(state);
		return cosf(val1);
	}
END_FUNC

BEGIN_FUNC(DSin, "sin");
	DSin()
	{
		mParam.push_back(new DFuncParam("mVal1", Float));
	}
	
	virtual float Evaluate(DFunctionState& state)
	{
		float val1 = mParam[0]->Evaluate(state);
		return sinf(val1);
	}
END_FUNC

BEGIN_FUNC(DTan, "tan");
	DTan()
	{
		mParam.push_back(new DFuncParam("mVal1", Float));
	}
	
	virtual float Evaluate(DFunctionState& state)
	{
		float val1 = mParam[0]->Evaluate(state);
		return tanf(val1);
	}
END_FUNC

BEGIN_FUNC(DPow, "pow");
	DPow()
	{
		mParam.push_back(new DFuncParam("mVal1", Float));
		mParam.push_back(new DFuncParam("mPower", Float));
	}
	
	virtual float Evaluate(DFunctionState& state)
	{
		float val1 = mParam[0]->Evaluate(state);
		float power = mParam[1]->Evaluate(state);
		return powf(val1, power);
	}
END_FUNC

BEGIN_FUNC(DRound, "round");
	DRound()
	{
		mParam.push_back(new DFuncParam("mVal1", Float));
	}
	
	virtual float Evaluate(DFunctionState& state)
	{
		float val1 = mParam[0]->Evaluate(state);
		return floor(val1 + 0.5f);
	}
END_FUNC

BEGIN_FUNC(DMad, "mad");
	DMad()
	{
		mParam.push_back(new DFuncParam("mVal1", Float));
		mParam.push_back(new DFuncParam("mArg1", Float));
		mParam.push_back(new DFuncParam("mArg2", Float));
	}
	
	virtual float Evaluate(DFunctionState& state)
	{
		float val1 = mParam[0]->Evaluate(state);
		float arg1 = mParam[1]->Evaluate(state);
		float arg2 = mParam[2]->Evaluate(state);
		return val1*arg1 + arg2;
	}
END_FUNC

BEGIN_FUNC(DFloor, "floor");
	DFloor()
	{
		mParam.push_back(new DFuncParam("mVal1", Float));
	}
	
	virtual float Evaluate(DFunctionState& state)
	{
		float val1 = mParam[0]->Evaluate(state);
		return floor(val1);
	}
END_FUNC

BEGIN_VECFUNC(DDistCosX, "cosx");
	DDistCosX()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRep1", Float));
	}
	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		float repX = mParam[1]->Evaluate(state);

		rPos[0] = cosf((rPos[0]-1.0f)*0.5f*3.141592f/repX);

		return rPos;
	}
END_FUNC

class DPerlin
{
public:
	float GetPerlin(float pos, float rep, float scale, int seed, int octaves, float weightingMultiplier)
	{
		float normalX = pos*rep;
		//float absX = abs(smod(normalX, 1.0));

		float sum=0;
		float weighting = scale;

		for (int i=0; i<octaves; i++)
		{
			int perlinScale = 2<<(i+1);
			int x0 = imod(rounddown(normalX*float(perlinScale)), perlinScale);
			float remainder = normalX*float(perlinScale) - float(rounddown(normalX*float(perlinScale)));
			int x1 = imod(x0+1, perlinScale);

			float fx0 = GetValue(x0, seed, 0, i);
			float fx1 = GetValue(x1, seed, 0, i);

			float ft = remainder * 3.1415927f;
			float f = (1 - cosf(ft)) * .5f;

			sum += (fx0*(1-f) + fx1*f) * weighting;
			weighting *= weightingMultiplier;
		}

		if (sum<-1) sum=-1;
		if (sum>1) sum=1;

		return sum;
	}

	float GetPerlin2d(float posx, float posy, float rep, float scale, int seed, int octaves, float weightingMultiplier)
	{
		float normalX = posx*rep;
		float normalY = posy*rep;
		float sum=0;
		float weighting = scale;

		for (int i=0; i<octaves; i++)
		{
			int perlinScale = 2<<(i+1);

			// calculate x axis position and y axis position
			int x0 = imod(rounddown(normalX*float(perlinScale)), perlinScale);
			float remainder = normalX*float(perlinScale) - float(rounddown(normalX*float(perlinScale)));
			int x1 = imod(x0+1, perlinScale);

			int y0 = imod(rounddown(normalY*float(perlinScale)), perlinScale);
			float remaindery = normalY*float(perlinScale) - float(rounddown(normalY*float(perlinScale)));
			int y1 = imod(y0+1, perlinScale);

			float fx0y0 = GetValue(x0 + perlinScale*y0, seed, 0, i);
			float fx1y0 = GetValue(x1 + perlinScale*y0, seed, 0, i);
			float fx0y1 = GetValue(x0 + perlinScale*y1, seed, 0, i);
			float fx1y1 = GetValue(x1 + perlinScale*y1, seed, 0, i);

			float ftx = remainder * 3.1415927f;
			float fx = (1 - cosf(ftx)) * .5f;
			float fty = remaindery * 3.1415927f;
			float fy = (1 - cosf(fty)) * .5f;

			sum += ((fx0y0*(1-fx) + fx1y0*fx) * (1-fy)
				+ (fx0y1*(1-fx) + fx1y1*fx) * fy)
				* weighting;
			weighting *= weightingMultiplier;
		}

		if (sum<-1) sum=-1;
		if (sum>1) sum=1;

		return sum;
	}

	float GetTurb3d(float posx, float posy, float posz, float rep, float scale, int seed, int octaves, float weightingMultiplier)
	{
		float normalX = posx*rep;
		float normalY = posy*rep;
		float normalZ = posz*rep;
		float sum=0;
		float weighting = scale;
		float val;

		for (int i=0; i<octaves; i++)
		{
			int perlinScale = 2<<(i+1);

			// calculate x axis position and y axis position
			int x0 = imod(rounddown(normalX*float(perlinScale)), perlinScale);
			float remainder = normalX*float(perlinScale) - float(rounddown(normalX*float(perlinScale)));
			int x1 = imod(x0+1, perlinScale);

			int y0 = imod(rounddown(normalY*float(perlinScale)), perlinScale);
			float remaindery = normalY*float(perlinScale) - float(rounddown(normalY*float(perlinScale)));
			int y1 = imod(y0+1, perlinScale);

			int z0 = imod(rounddown(normalZ*float(perlinScale)), perlinScale);
			float remainderz = normalZ*float(perlinScale) - float(rounddown(normalZ*float(perlinScale)));
			int z1 = imod(z0+1, perlinScale);

			float fx0y0z0 = GetValue(x0 + perlinScale*y0 + perlinScale*perlinScale*z0, seed, 0, i);
			float fx1y0z0 = GetValue(x1 + perlinScale*y0 + perlinScale*perlinScale*z0, seed, 0, i);
			float fx0y1z0 = GetValue(x0 + perlinScale*y1 + perlinScale*perlinScale*z0, seed, 0, i);
			float fx1y1z0 = GetValue(x1 + perlinScale*y1 + perlinScale*perlinScale*z0, seed, 0, i);
			float fx0y0z1 = GetValue(x0 + perlinScale*y0 + perlinScale*perlinScale*z1, seed, 0, i);
			float fx1y0z1 = GetValue(x1 + perlinScale*y0 + perlinScale*perlinScale*z1, seed, 0, i);
			float fx0y1z1 = GetValue(x0 + perlinScale*y1 + perlinScale*perlinScale*z1, seed, 0, i);
			float fx1y1z1 = GetValue(x1 + perlinScale*y1 + perlinScale*perlinScale*z1, seed, 0, i);

			float ftx = remainder * 3.1415927f;
			float fx = (1 - cosf(ftx)) * .5f;
			float fty = remaindery * 3.1415927f;
			float fy = (1 - cosf(fty)) * .5f;
			float ftz = remainderz * 3.1415927f;
			float fz = (1 - cosf(ftz)) * .5f;

			val = ((((fx0y0z0*(1-fx) + fx1y0z0*fx) * (1-fy)
				+ (fx0y1z0*(1-fx) + fx1y1z0*fx) * fy) * (1-fz))
				+ (((fx0y0z1*(1-fx) + fx1y0z1*fx) * (1-fy)
				+ (fx0y1z1*(1-fx) + fx1y1z1*fx) * fy) * fz))
				* weighting;
			sum += fabsf(val);
			weighting *= weightingMultiplier;
		}

		if (sum<-1) sum=-1;
		if (sum>1) sum=1;

		return sum;
	}

	float GetPerlin3d(float posx, float posy, float posz, float rep, float scale, int seed, int octaves, float weightingMultiplier)
	{
		float normalX = posx*rep;
		float normalY = posy*rep;
		float normalZ = posz*rep;
		float sum=0;
		float weighting = scale;

		for (int i=0; i<octaves; i++)
		{
			int perlinScale = 2<<(i+1);

			// calculate x axis position and y axis position
			int x0 = imod(rounddown(normalX*float(perlinScale)), perlinScale);
			float remainder = normalX*float(perlinScale) - float(rounddown(normalX*float(perlinScale)));
			int x1 = imod(x0+1, perlinScale);

			int y0 = imod(rounddown(normalY*float(perlinScale)), perlinScale);
			float remaindery = normalY*float(perlinScale) - float(rounddown(normalY*float(perlinScale)));
			int y1 = imod(y0+1, perlinScale);

			int z0 = imod(rounddown(normalZ*float(perlinScale)), perlinScale);
			float remainderz = normalZ*float(perlinScale) - float(rounddown(normalZ*float(perlinScale)));
			int z1 = imod(z0+1, perlinScale);

			float fx0y0z0 = GetValue(x0 + perlinScale*y0 + perlinScale*perlinScale*z0, seed, 0, i);
			float fx1y0z0 = GetValue(x1 + perlinScale*y0 + perlinScale*perlinScale*z0, seed, 0, i);
			float fx0y1z0 = GetValue(x0 + perlinScale*y1 + perlinScale*perlinScale*z0, seed, 0, i);
			float fx1y1z0 = GetValue(x1 + perlinScale*y1 + perlinScale*perlinScale*z0, seed, 0, i);
			float fx0y0z1 = GetValue(x0 + perlinScale*y0 + perlinScale*perlinScale*z1, seed, 0, i);
			float fx1y0z1 = GetValue(x1 + perlinScale*y0 + perlinScale*perlinScale*z1, seed, 0, i);
			float fx0y1z1 = GetValue(x0 + perlinScale*y1 + perlinScale*perlinScale*z1, seed, 0, i);
			float fx1y1z1 = GetValue(x1 + perlinScale*y1 + perlinScale*perlinScale*z1, seed, 0, i);

			float ftx = remainder * 3.1415927f;
			float fx = (1 - cosf(ftx)) * .5f;
			float fty = remaindery * 3.1415927f;
			float fy = (1 - cosf(fty)) * .5f;
			float ftz = remainderz * 3.1415927f;
			float fz = (1 - cosf(ftz)) * .5f;

			sum += ((((fx0y0z0*(1-fx) + fx1y0z0*fx) * (1-fy)
				+ (fx0y1z0*(1-fx) + fx1y1z0*fx) * fy) * (1-fz))
				+ (((fx0y0z1*(1-fx) + fx1y0z1*fx) * (1-fy)
				+ (fx0y1z1*(1-fx) + fx1y1z1*fx) * fy) * fz))
				* weighting;
			weighting *= weightingMultiplier;
		}

		if (sum<-1) sum=-1;
		if (sum>1) sum=1;

		return sum;
	}

private:
	float GetValue(int x, int seed, int axis, int octave)
	{
		int val = x + axis*789221 + octave*15731 + seed*761;
	    val = (val<<13) ^ val;
		return 1.0f - ( float(val * (val * val * 15731 + 789221) + 1376312589 & 0x7fffffff) / 1073741824.0f);
	}
	int imod(int arg, int mod)
	{
		int ret = arg % mod;
		if (ret<0) ret += mod;
		return ret;
	}
	int rounddown(float val)
	{
		if (val<0)
			return int(val)-1;
		else
			return int(val);
	}
};

BEGIN_VECFUNC(DDistPerlinX, "perlinx");
	DDistPerlinX()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRep1", Float));
		mParam.push_back(new DFuncParam("mSeed", Float));
		mParam.push_back(new DFuncParam("mOctaves", Float));
		mParam.push_back(new DFuncParam("mWeightingMultiplier", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		float repX = mParam[1]->Evaluate(state);
		int seed = (int)mParam[2]->Evaluate(state);
		int octaves = (int)mParam[3]->Evaluate(state);
		float weightingMultiplier = mParam[4]->Evaluate(state);

		/*
		for (float i=-1; i<1; i+=0.01f)
		{
			std::wstringstream ss;
			ss << L"Perlin (" << i << L", " << GetPerlin(0.5f*(i+1), repX, 0) << L"\n";
			OutputDebugString(ss.str().c_str());
		}
		*/

		rPos[0] = mPerlin.GetPerlin(0.5f*(rPos[0]+1), repX, 0, seed, octaves, weightingMultiplier);

		return rPos;
	}
	
	DPerlin mPerlin;
END_FUNC

BEGIN_FUNC(DDistPerlinXZ, "perlinxz");
	DDistPerlinXZ()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRep", Float));
		mParam.push_back(new DFuncParam("mInitialWeight", Float));
		mParam.push_back(new DFuncParam("mSeed", Float));
		mParam.push_back(new DFuncParam("mOctaves", Float));
		mParam.push_back(new DFuncParam("mWeightingMultiplier", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		float repX = mParam[1]->Evaluate(state);
		float weight = mParam[2]->Evaluate(state);
		int seed = (int)mParam[3]->Evaluate(state);
		int octaves = (int)mParam[4]->Evaluate(state);
		float weightingMultiplier = mParam[5]->Evaluate(state);
		return mPerlin.GetPerlin2d(0.5f*(rPos[0]+1), 0.5f*(rPos[2]+1), repX, weight, seed, octaves, weightingMultiplier);
	}
	
	DPerlin mPerlin;
END_FUNC

BEGIN_FUNC(DDistPerlinXYZ, "perlinxyz");
	DDistPerlinXYZ()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRep", Float));
		mParam.push_back(new DFuncParam("mInitialWeight", Float));
		mParam.push_back(new DFuncParam("mSeed", Float));
		mParam.push_back(new DFuncParam("mOctaves", Float));
		mParam.push_back(new DFuncParam("mWeightingMultiplier", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		float repX = mParam[1]->Evaluate(state);
		float weight = mParam[2]->Evaluate(state);
		int seed = (int)mParam[3]->Evaluate(state);
		int octaves = (int)mParam[4]->Evaluate(state);
		float weightingMultiplier = mParam[5]->Evaluate(state);
		return mPerlin.GetPerlin3d(0.5f*(rPos[0]+1), 0.5f*(rPos[1]+1), 0.5f*(rPos[2]+1), repX, weight, seed, octaves, weightingMultiplier);
	}
	
	DPerlin mPerlin;
END_FUNC

BEGIN_FUNC(DDistTurb3d, "turb3d");
	DDistTurb3d()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRep", Float));
		mParam.push_back(new DFuncParam("mInitialWeight", Float));
		mParam.push_back(new DFuncParam("mSeed", Float));
		mParam.push_back(new DFuncParam("mOctaves", Float));
		mParam.push_back(new DFuncParam("mWeightingMultiplier", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		float repX = mParam[1]->Evaluate(state);
		float weight = mParam[2]->Evaluate(state);
		int seed = (int)mParam[3]->Evaluate(state);
		int octaves = (int)mParam[4]->Evaluate(state);
		float weightingMultiplier = mParam[5]->Evaluate(state);
		return mPerlin.GetTurb3d(0.5f*(rPos[0]+1), 0.5f*(rPos[1]+1), 0.5f*(rPos[2]+1), repX, weight, seed, octaves, weightingMultiplier);
	}
	
	DPerlin mPerlin;
END_FUNC

BEGIN_VECFUNC(DAddVec, "add");
	DAddVec()
	{
		mParam.push_back(new DFuncParam("mVal1", Vec));
		mParam.push_back(new DFuncParam("mVal2", Vec));
	}
	
	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 rVal1 = mParam[0]->EvaluateVec(state);
		DVector3 rVal2 = mParam[1]->EvaluateVec(state);
		return rVal1 + rVal2;
	}
END_FUNC

BEGIN_VECFUNC(DDistDivVec, "div");
	DDistDivVec()
	{
		mParam.push_back(new DFuncParam("mVal1", Vec));
		mParam.push_back(new DFuncParam("mVal2", Float));
	}
	
	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 rVal1 = mParam[0]->EvaluateVec(state);
		float rVal2 = mParam[1]->Evaluate(state);
		return rVal1 / rVal2;
	}
END_FUNC

BEGIN_FUNC(DDistRep, "rep");
	DDistRep()
	{
		mParam.push_back(new DFuncParam("mVal", Float));
		mParam.push_back(new DFuncParam("mRep", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		float val = mParam[0]->Evaluate(state);
		float rep = mParam[1]->Evaluate(state);
		val = smod((val+rep),rep*2)-rep;
		return val;
	}
END_FUNC

BEGIN_VECFUNC(DDistRepX, "repx");
	DDistRepX()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRep1", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		float repX = mParam[1]->Evaluate(state);
		rPos[0] = smod((rPos[0]+repX),repX*2)-repX;
		return rPos;
	}
END_FUNC

BEGIN_FUNC(DDistVoronoi, "voronoi");
	DDistVoronoi()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mSeed", Float));
		mParam.push_back(new DFuncParam("mNum", Float));
		mParam.push_back(new DFuncParam("mScale", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		unsigned int num = unsigned int(mParam[2]->Evaluate(state));
		DVector3 *centres = DStaticCentres::Get().GetCentres(unsigned int(mParam[1]->Evaluate(state)), num);

		DVector3 pos = mParam[0]->EvaluateVec(state);
		pos[0] = smod(pos[0]+1,2)-1;
		pos[1] = smod(pos[1]+1,2)-1;
		pos[2] = smod(pos[2]+1,2)-1;

		float dist = 2;
		float dist2 = 2;
		for (unsigned int i=0; i<num; i++)
		{
			float tempdistx = fabsf(centres[i][0] - pos[0]);
			if (tempdistx>1) tempdistx = 2 - tempdistx;
			float tempdisty = fabsf(centres[i][1] - pos[1]);
			if (tempdisty>1) tempdisty = 2 - tempdisty;
			float tempdistz = fabsf(centres[i][2] - pos[2]);
			if (tempdistz>1) tempdistz = 2 - tempdistz;
			float tempdist = sqrtf(tempdistx*tempdistx + tempdisty*tempdisty + tempdistz*tempdistz);
			if (tempdist<dist) {dist2=dist;dist=tempdist;}
			else if (tempdist<dist2) {dist2=tempdist;}
		}
		dist = clamp((dist2-dist)*0.5f*mParam[3]->Evaluate(state), 0, 1);
		return dist;
	}
END_FUNC

BEGIN_FUNC(DDistWaves, "waves");
	DDistWaves()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mSeed", Float));
		mParam.push_back(new DFuncParam("mWind", Vec));
		mParam.push_back(new DFuncParam("mResolution", Float));
	}

	float CubicInterpolate (float v0, float v1, float v2, float v3, float x)
	{
		return v1 + 0.5f * x*(v2 - v0 + x*(2.0f*v0 - 5.0f*v1 + 4.0f*v2 - v3 + x*(3.0f*(v1 - v2) + v3 - v0)));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		unsigned int seed = unsigned int(mParam[1]->Evaluate(state));
		DVector3 wind = mParam[2]->EvaluateVec(state);
		int resolution = int(powf(2.0f, log(mParam[3]->Evaluate(state))/log(2.f)));

		float* mData = DTextureCache::Get().GetTexture(seed, DVector2(wind[0], wind[1]), resolution);

		DVector3 pos = mParam[0]->EvaluateVec(state);
		pos *= float(resolution);
		pos[0] = fmodf(pos[0], float(resolution));
		if (pos[0]<0) pos[0] += resolution;
		pos[2] = fmodf(pos[2], float(resolution));
		if (pos[2]<0) pos[2] += resolution;

		int x = int(pos[0])%resolution; // very very rarely fmodf(1023.9999,1024)+1024 == 1024
		int y = int(pos[2])%resolution;
		int xm1 = (x-1)%resolution;
		if (xm1<0) xm1 += resolution;
		int ym1 = (y-1)%resolution;
		if (ym1<0) ym1 += resolution;
		int xp1 = (x+1)%resolution;
		int yp1 = (y+1)%resolution;
		int xp2 = (x+2)%resolution;
		int yp2 = (y+2)%resolution;
		float remainderx = pos[0] - float(x);
		float remaindery = pos[2] - float(y);

		float fxmym = mData[xm1 + ym1*resolution];
		float fx0ym = mData[x + ym1*resolution];
		float fx1ym = mData[xp1 + ym1*resolution];
		float fx2ym = mData[xp2 + ym1*resolution];
		float fxmy0 = mData[xm1 + y*resolution];
		float fx0y0 = mData[x + y*resolution];
		float fx1y0 = mData[xp1 + y*resolution];
		float fx2y0 = mData[xp2 + y*resolution];
		float fxmy1 = mData[xm1 + yp1*resolution];
		float fx0y1 = mData[x + yp1*resolution];
		float fx1y1 = mData[xp1 + yp1*resolution];
		float fx2y1 = mData[xp2 + yp1*resolution];
		float fxmy2 = mData[xm1 + yp2*resolution];
		float fx0y2 = mData[x + yp2*resolution];
		float fx1y2 = mData[xp1 + yp2*resolution];
		float fx2y2 = mData[xp2 + yp2*resolution];

		float v0 = CubicInterpolate(fxmym, fx0ym, fx1ym, fx2ym, remainderx);
		float v1 = CubicInterpolate(fxmy0, fx0y0, fx1y0, fx2y0, remainderx);
		float v2 = CubicInterpolate(fxmy1, fx0y1, fx1y1, fx2y1, remainderx);
		float v3 = CubicInterpolate(fxmy2, fx0y2, fx1y2, fx2y2, remainderx);

		return CubicInterpolate(v0, v1, v2, v3, remaindery);
/*

		float fx = remainderx;// * 3.1415927f;
//		float fx = (1 - cosf(ftx)) * .5f;
		float fy = remaindery;// * 3.1415927f;
	//	float fy = (1 - cosf(fty)) * .5f;

		return ((fx0y0*(1-fx) + fx1y0*fx) * (1-fy)
				+ (fx0y1*(1-fx) + fx1y1*fx) * fy);*/
	}
END_FUNC

BEGIN_VECFUNC(DDistBoxFold, "boxfold");
	DDistBoxFold()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mSideLength", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		float sideLength = mParam[1]->Evaluate(state);
		if (rPos[0]<-sideLength) rPos[0] = -2*sideLength - rPos[0];
		else if (rPos[0]>sideLength) rPos[0] = 2*sideLength - rPos[0];
		if (rPos[1]<-sideLength) rPos[1] = -2*sideLength - rPos[1];
		else if (rPos[1]>sideLength) rPos[1] = 2*sideLength - rPos[1];
		if (rPos[2]<-sideLength) rPos[2] = -2*sideLength - rPos[2];
		else if (rPos[2]>sideLength) rPos[2] = 2*sideLength - rPos[2];
		return rPos;
		//p = clamp(p,-L,L)*2-p
	}
END_FUNC

BEGIN_VECFUNC(DDistSphereFold, "spherefold");
	DDistSphereFold()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRadius", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		float foldRadius = mParam[1]->Evaluate(state);
		float foldRadiusSquared = foldRadius * foldRadius;
		float radiusSquared = rPos.MagnitudeSquared();
		float radius = sqrt(radiusSquared);
		if (radiusSquared<foldRadiusSquared)
		{
			rPos = rPos * (foldRadius/radius);
			rPos = rPos * (1 + (foldRadius-radius)/foldRadius);
//			rPos = rPos * (foldRadiusSquared / radiusSquared);
		}
		return rPos;
		//float r = length(p);
		//if (r<R) p=p*R*R/(r*r);
	}
END_FUNC

BEGIN_VECFUNC(DDistOuterSphereFold, "outerspherefold");
	DDistOuterSphereFold()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRadius", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		float foldRadius = mParam[1]->Evaluate(state);
		float foldRadiusSquared = foldRadius * foldRadius;
		float radiusSquared = rPos.MagnitudeSquared();
		if (radiusSquared>foldRadiusSquared)
		{
			float radius = sqrt(radiusSquared);
			float outerDistance = radius - foldRadius;
			outerDistance = abs(foldRadius - smod(outerDistance, foldRadius*2));
			rPos = rPos * (foldRadius/radius);
			rPos = rPos * (outerDistance/foldRadius);
		}
		return rPos;
	}
END_FUNC

BEGIN_FUNC(DDistFold, "fold");
	DDistFold()
	{
		mParam.push_back(new DFuncParam("mVal", Float));
		mParam.push_back(new DFuncParam("mRep", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		float val = mParam[0]->Evaluate(state);
		float rep = mParam[1]->Evaluate(state);
		val = abs(smod((val+rep),rep*4)-rep*2)-rep;
		return val;
	}
END_FUNC

BEGIN_FUNC(DDistSingleFold, "singlefold");
	DDistSingleFold()
	{
		mParam.push_back(new DFuncParam("mVal", Float));
		mParam.push_back(new DFuncParam("mFold", Float));
	}

	virtual float Evaluate(DFunctionState& state)
	{
		float val = mParam[0]->Evaluate(state);
		float fold = mParam[1]->Evaluate(state);
		if (val>fold) val = fold*2 - val;
		return val;
	}
END_FUNC

BEGIN_VECFUNC(DDistSingleFold3D, "singlefold3d");
	DDistSingleFold3D()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mPlanePoint", Vec));
		mParam.push_back(new DFuncParam("mNormal", Vec));
	}

	int mCacheMode;
	DVector3 mCacheNormal;

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 pos = mParam[0]->EvaluateVec(state);
		DVector3 planepoint = mParam[1]->EvaluateVec(state);
		DVector3 normal = mParam[2]->EvaluateVec(state);

		if (planepoint[0]==0.0 && planepoint[1]==0.0 && planepoint[2]==0.0)
		{
			if (normal[0]==mCacheNormal[0] && normal[1]==mCacheNormal[1] && normal[2]==mCacheNormal[2])
			{
				switch (mCacheMode)
				{
				case 0:
					{
					float distance = -pos.Dot(mCacheNormal);
					if (distance>0)
					{
						pos += normal*distance*2;
					}
					}
					break;
				case 1:
					if(pos[0]+pos[1]<0){float tmp=-pos[1];pos[1]=-pos[0];pos[0]=tmp;}
					break;
				case 2:
					if(pos[0]+pos[2]<0){float tmp=-pos[2];pos[2]=-pos[0];pos[0]=tmp;}
					break;
				case 3:
					if(pos[1]+pos[2]<0){float tmp=-pos[2];pos[2]=-pos[1];pos[1]=tmp;}
					break;
				}

				return pos;
			}
			else
			{
				mCacheNormal = normal;
				mCacheMode = 0;
				if (mCacheNormal[0] == 1 && mCacheNormal[1] == 1 && mCacheNormal[2] == 0) {mCacheMode = 1;}
				if (mCacheNormal[0] == 1 && mCacheNormal[1] == 0 && mCacheNormal[2] == 1) {mCacheMode = 2;}
				if (mCacheNormal[0] == 0 && mCacheNormal[1] == 1 && mCacheNormal[2] == 1) {mCacheMode = 3;}
			}
		}

		normal.Normalise();
		DVector3 planevec = planepoint - pos;
		float distance = planevec.Dot(normal);
		if (distance>0)
		{
			return pos + normal*distance*2;
		}
		return pos;
	}
END_FUNC

BEGIN_VECFUNC(DDistRotateFold, "rotatefold");
	DDistRotateFold()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mDegrees", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 pos = mParam[0]->EvaluateVec(state);
		float rep = mParam[1]->Evaluate(state);

		float length = sqrtf(pos[0]*pos[0] + pos[2]*pos[2]);
		float angle = acosf(pos[2]/length)*180/PI;
		if (pos[0]<0) angle = -angle;

		float newangle = abs(smod((angle+rep*1.5f),rep*2)-rep)-rep*0.5f;
		newangle = newangle*PI/180;
		
		return DVector3(length*sin(newangle), pos[1], length*cos(newangle));
	}
END_FUNC

BEGIN_VECFUNC(DDistCylinderFold, "cylinderfold");
	DDistCylinderFold()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mCentreLine", Vec));
		mParam.push_back(new DFuncParam("mDistance", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 pos = mParam[0]->EvaluateVec(state);
		DVector3 centreLine = mParam[1]->EvaluateVec(state);
		float foldDistance = mParam[2]->Evaluate(state);

		// http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
		float t = pos.Dot(centreLine) / centreLine.MagnitudeSquared();

		DVector3 reflectPos = centreLine * t;
		float distance = (pos - reflectPos).Magnitude();

		if (distance < foldDistance)
		{
			DVector3 toVector = (pos-reflectPos) * 2 * (foldDistance-distance)/distance;
			DVector3 newPos = pos + toVector;
			return newPos;
		}
		else
		{
			return pos;
		}
	}
END_FUNC

BEGIN_VECFUNC(DDistFoldX, "foldx");
	DDistFoldX()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRep1", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		float repX = mParam[1]->Evaluate(state);
		rPos[0] = (abs(repX*2 - smod((rPos[0]+1),repX*4))-repX) / repX;
		return rPos;
	}
END_FUNC

BEGIN_VECFUNC(DDistRepY, "repy");
	DDistRepY()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRep1", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		float repY = mParam[1]->Evaluate(state);
		rPos[1] = smod(rPos[1]+repY,repY*2)-repY;
		return rPos;
	}
END_FUNC

BEGIN_VECFUNC(DDistRepZ, "repz");
	DDistRepZ()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRep1", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		float repZ = mParam[1]->Evaluate(state);
		rPos[2] = smod(rPos[2]+repZ,repZ*2)-repZ;
		return rPos;
	}
END_FUNC
	 
BEGIN_VECFUNC(DDistRepXYZ, "repxyz");
	DDistRepXYZ()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRep1", Vec));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 rPos = mParam[0]->EvaluateVec(state);
		DVector3 rep = mParam[1]->EvaluateVec(state);
		rPos[0] = smod(rPos[0]+rep[0],rep[0]*2)-rep[0];
		rPos[1] = smod(rPos[1]+rep[1],rep[1]*2)-rep[1];
		rPos[2] = smod(rPos[2]+rep[2],rep[2]*2)-rep[2];
		return rPos;
	}
END_FUNC

BEGIN_VECFUNC(DDistSubVec, "sub");
	DDistSubVec()
	{
		mParam.push_back(new DFuncParam("mX", Vec));
		mParam.push_back(new DFuncParam("mX", Vec));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		return mParam[0]->EvaluateVec(state) - mParam[1]->EvaluateVec(state);
	}
END_VECFUNC

BEGIN_VECFUNC(DDistMulVec, "mul");
	DDistMulVec()
	{
		mParam.push_back(new DFuncParam("mVec", Vec));
		mParam.push_back(new DFuncParam("mMul", Vec));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		return mParam[0]->EvaluateVec(state) * mParam[1]->EvaluateVec(state);
	}
END_VECFUNC

BEGIN_VECFUNC(DDistMulVecF, "mul");
	DDistMulVecF()
	{
		mParam.push_back(new DFuncParam("mVec", Vec));
		mParam.push_back(new DFuncParam("mMul", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		return mParam[0]->EvaluateVec(state) * mParam[1]->Evaluate(state);
	}
END_VECFUNC

BEGIN_VECFUNC(DDistVec, "vec");
	DDistVec()
	{
		mParam.push_back(new DFuncParam("mX", Float));
		mParam.push_back(new DFuncParam("mY", Float));
		mParam.push_back(new DFuncParam("mZ", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		float x = mParam[0]->Evaluate(state);
		float y = mParam[1]->Evaluate(state);
		float z = mParam[2]->Evaluate(state);
		return DVector3(x, y, z);
	}
END_VECFUNC

BEGIN_VECFUNC(DDistPos, "pos");
	DDistPos()
	{
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		return state.mPosition;
	}
END_VECFUNC

BEGIN_VECFUNC(DDistRotX, "rotx");
	DDistRotX()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRot", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 mPos = mParam[0]->EvaluateVec(state);
		float rot = mParam[1]->Evaluate(state);

		DMatrix4 mat;
		mat.MakeFromRPY(0, -3.14159265f*rot/180,0);
		return mat * mPos;
	}
END_VECFUNC

BEGIN_VECFUNC(DDistRotY, "roty");
	DDistRotY()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRot", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 mPos = mParam[0]->EvaluateVec(state);
		float rot = mParam[1]->Evaluate(state);

		DMatrix4 mat;
		mat.MakeFromRPY(-3.14159265f*rot/180,0,0);
		return mat * mPos;
	}
END_VECFUNC

BEGIN_VECFUNC(DDistRotZ, "rotz");
	DDistRotZ()
	{
		mParam.push_back(new DFuncParam("mPos", Vec));
		mParam.push_back(new DFuncParam("mRot", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 mPos = mParam[0]->EvaluateVec(state);
		float rot = mParam[1]->Evaluate(state);

		DMatrix4 mat;
		mat.MakeFromRPY(0,0,-3.14159265f*rot/180);
		return mat * mPos;
	}
END_VECFUNC

BEGIN_VECFUNC(DLerp, "lerp");
	DLerp()
	{
		mParam.push_back(new DFuncParam("mVec1", Vec));
		mParam.push_back(new DFuncParam("mVec2", Vec));
		mParam.push_back(new DFuncParam("mAmount", Float));
	}

	virtual DVector3 Evaluate(DFunctionState& state)
	{
		DVector3 mVec1 = mParam[0]->EvaluateVec(state);
		DVector3 mVec2 = mParam[1]->EvaluateVec(state);
		float mAmount = clamp(mParam[2]->Evaluate(state),0,1);
		float invAmount = 1-mAmount;

		return DVector3(mVec1[0]*invAmount + mVec2[0]*mAmount,
			mVec1[1]*invAmount + mVec2[1]*mAmount,
			mVec1[2]*invAmount + mVec2[2]*mAmount);
	}
END_VECFUNC

BEGIN_NULLFUNC(DSet, "set");
	DSet()
	{
		mParam.push_back(new DFuncParam("mTarget", Float));
		mParam.push_back(new DFuncParam("mValue", Float));
	}

	virtual void Evaluate(DFunctionState& state)
	{
		((DDistFloatVar*)(mParam[0]->mVar))->Set(state, mParam[1]->Evaluate(state));
	}
END_NULLFUNC

BEGIN_NULLFUNC(DSetV, "set");
	DSetV()
	{
		mParam.push_back(new DFuncParam("mTarget", Vec));
		mParam.push_back(new DFuncParam("mValue", Vec));
	}

	virtual void Evaluate(DFunctionState& state)
	{
		((DDistVectorVar*)(mParam[0]->mVar))->Set(state, mParam[1]->EvaluateVec(state));

	}
END_NULLFUNC

#endif // DISTFUNCTIONS_H