#ifndef DISTFUNCTIONS_H
#define DISTFUNCTIONS_H

#include "FuncCore.h"
#include "FuncFactory.h"
#include "Vector3.h"
#include "Matrix4.h"
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

BEGIN_VECFUNC(DDistVectorVar, "vecVar");
	DDistVectorVar()
	{
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
            }

            mpos = mpos * mScale + c;
            dr = dr * mScale + 1.0f;
        }
        return mpos.Magnitude() / abs(dr);
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
		for (int i = 0; i < Iterations ; i++) {
			r = z.Magnitude();
			if (r>Bailout) break;

			// convert to polar coordinates
			float theta = acos(z[2]/r);
			float phi = atan2(z[1],z[0]);
			dr =  pow( r, Power-1.0f)*Power*dr + 1.0f;

			// scale and rotate the point
			float zr = pow( r,Power);
			theta = theta*Power;
			phi = phi*Power;

			// convert back to cartesian coordinates
			z = DVector3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta))*zr + pos;
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
	}

	void rotate1(DVector3& pos, const DVector3& rot)
	{
		DMatrix4 mat;
		mat.MakeFromRPY(rot[0],rot[1],rot[2]);
		pos = mat * pos;
	
	}

	virtual float Evaluate(DFunctionState& state)
	{
		DVector3 pos = mParam[0]->EvaluateVec(state);
		DVector3 rot1 = mParam[1]->EvaluateVec(state);
		DVector3 rot2 = mParam[2]->EvaluateVec(state);
		DVector3 offset = mParam[3]->EvaluateVec(state);
		int iterations = int(0.5f + mParam[4]->Evaluate(state));
		float scale = mParam[5]->Evaluate(state);

		float bailout = 1000;
		float r=pos.MagnitudeSquared();
		int i;
		for(i=0;i<iterations && r<bailout;i++){
			//Folding... These are some of the symmetry planes of the tetrahedron
			float tmp;

			rotate1(pos, rot1);

			if(pos[0]+pos[1]<0){tmp=-pos[1];pos[1]=-pos[0];pos[0]=tmp;}
			if(pos[0]+pos[2]<0){tmp=-pos[2];pos[2]=-pos[0];pos[0]=tmp;}
			if(pos[1]+pos[2]<0){tmp=-pos[2];pos[2]=-pos[1];pos[1]=tmp;}
      
			rotate1(pos, rot2);

			//Stretche about the point [1,1,1]*(scale-1)/scale; The "(scale-1)/scale" is here in order to keep the size of the fractal constant wrt scale
			pos[0]=scale*pos[0]-offset[0]*(scale-1);//equivalent to: x=scale*(x-cx); where cx=(scale-1)/scale;
			pos[1]=scale*pos[1]-offset[1]*(scale-1);
			pos[2]=scale*pos[2]-offset[2]*(scale-1);
			r=pos.MagnitudeSquared();
		}	
		return (sqrtf(r)-2)*powf(scale,-float(i));//the estimated distance
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
		float mAmount = mParam[2]->Evaluate(state);
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