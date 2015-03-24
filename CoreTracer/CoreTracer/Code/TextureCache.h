#pragma once

#include <vector>
#include "Vector3.h"
#include "Vector2.h"
#include "Random.h"

class DTexture
{
public:
	DTexture(unsigned int mSeed, DVector2 &wind, int resolution);
	~DTexture();

	float* GetTexture() {return mData;}
	void Initialise();
	DVector2& GetWind() {return mWind;}
	unsigned int GetSeed() {return mSeed;}
	int GetResolution() {return mResolution;}

private:
	unsigned int mSeed;
	DVector2 mWind;
	float* mData;
	int mResolution;
	DRandom mRandom;
};

class DTextureCache
{
public:
	~DTextureCache();

	static DTextureCache& Get();

	float* GetTexture(unsigned int seed, DVector2& wind, int resolution);

private:
	std::vector<DTexture*> mTextures;

};
