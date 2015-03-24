#include "stdafx.h"
#include "TextureCache.h"
#include "Vector2.h"
#include "FFT.h"

DTextureCache& DTextureCache::Get()
{
	static DTextureCache singleton;
	return singleton;
}

DTextureCache::~DTextureCache()
{
	for (unsigned int i=0; i<mTextures.size(); i++)
	{
		delete mTextures[i];
	}
	mTextures.clear();
}

float* DTextureCache::GetTexture(unsigned int seed, DVector2& wind, int resolution)
{
	for (unsigned i=0; i<mTextures.size(); i++)
	{
		if (seed==mTextures[i]->GetSeed() && resolution==mTextures[i]->GetResolution())
		{
			if (wind.x==mTextures[i]->GetWind().x
				&& wind.y==mTextures[i]->GetWind().y)
			{
				return mTextures[i]->GetTexture();
			}
		}
	}

	mTextures.push_back(new DTexture(seed, wind, resolution));
	mTextures[mTextures.size()-1]->Initialise();

	return mTextures[mTextures.size()-1]->GetTexture();
}

DVector2 rVtoGaussianRV(DVector3 rV)
{
	// http://stackoverflow.com/questions/5837572/generate-a-random-point-within-a-circle-uniformly
	float u = rV[0] + rV[1];
	float t = rV[2] * 2 * PI;
	if (u>1) u = 2-u;
	DVector2 circleRV = DVector2(u*cosf(t), u*sinf(t));
	float w = circleRV.GetLength();
	w = sqrtf((-2*logf(w)) / w);
	return DVector2(circleRV.x*w, circleRV.y*w);
}

float GetPhillips(DVector2 k, float A, DVector2 wind)
{
	float length = k.GetLength();

	if (length<0.000001f) return 0.0f;

	float length2 = length*length;
	float length4 = length2*length2;

	float k_dot_w = k.GetDot(wind) / (k.GetLength() * wind.GetLength());
	float k_dot_w2 = k_dot_w*k_dot_w;

	float wlength = wind.GetLength();
	float L = wlength * wlength / 9.81f;
	float L2 = L*L;

	float damping = 0.0001f;
	float l2 = L2*damping*damping;

	return A * exp(-1/(length2*L2)) / length4 * k_dot_w2 * exp(-length2*l2);
}

DTexture::DTexture(unsigned int seed, DVector2& wind, int resolution) : mRandom(seed), mSeed(seed), mData(NULL), mWind(wind), mResolution(resolution)
{
}

void DTexture::Initialise()
{
/*	mData = new float[256*256];

	DVector3 *waves = new DVector3[number];
	DVector2 *directions = new DVector2[number];
	for (int i=0; i<number; i++)
	{
		directions[i] = DVector2(mRandom.GetNextFloat()*2-1, mRandom.GetNextFloat()*2-1);
		directions[i].Normalise();
		DVector2 gaussian = rVtoGaussianRV(DVector3(mRandom.GetNextFloat(), mRandom.GetNextFloat(), mRandom.GetNextFloat()));

		float length = (gaussian.x+1) * 0.49 + 0.02f;
		float amplitude = sqrt(9.81*length);
		waves[i][0] = length;
		waves[i][1] = amplitude;
		waves[i][2] = 2*PI*mRandom.GetNextFloat();
	}

	for (int y=0; y<256; y++)
	{
		for (int x=0; x<256; x++)
		{
			float accumulator = 0;
			for (int i=0; i<number; i++)
			{
				DVector2 pos2 = DVector2(x/32.f, y/32.f);
				float dist = pos2.GetDot(directions[i]);
				accumulator += waves[i][1]*cos((dist/waves[i][0]) + PI*waves[i][2]);
			}
			mData[x+256*y] = accumulator;
		}
	}*/

	DVector2* mWaves = new DVector2[mResolution*mResolution];
	mData = new float[mResolution*mResolution];

	for (int y=0; y<mResolution; y++)
	{
		for (int x=0; x<mResolution; x++)
		{

/*			float amplitude = 0;
			float dx = x-128;
			float dy = y-128;
			float dist = sqrtf(dx*dx + dy*dy);
			if (dist<25) amplitude = (25.f-dist) / 25.f;
			amplitude = pow(amplitude,3);
			float phase = 2*PI*mRandom.GetNextFloat();
*/
			
			float amplitude = sqrtf(GetPhillips(DVector2(PI*(x-128.f)/128.f, PI*(y-128.f)/128.f), 1, mWind));
			float phase = 2*PI*mRandom.GetNextFloat();
			//			if (x==124 && y==128) amplitude = 10;
//			if (x==128 && y==128) amplitude = 10;
//			if (x==132 && y==128) amplitude = 10;
			mWaves[x+mResolution*y].x = amplitude * cos(phase);// * mRandom.GetNextFloat();
			mWaves[x+mResolution*y].y = amplitude * sin(phase);
		}
	}

	bool success = DFFT::FFT2D(mWaves, mResolution, mResolution, 1);

	for (int y=0; y<mResolution; y++)
	{
		for (int x=0; x<mResolution; x++)
		{
			float dx = mWaves[x+mResolution*y].x;
			float dy = mWaves[x+mResolution*y].y;
			mData[x+mResolution*y] = sqrtf(dx*dx+dy*dy);
		}
	}

	delete [] mWaves;
}

DTexture::~DTexture()
{
	delete [] mData;
	mData = NULL;
}