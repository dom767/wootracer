#pragma once

#include <vector>
#include "Vector3.h"
#include "Random.h"

class DCentreCollection
{
public:
	DCentreCollection(unsigned int mSeed);

	DVector3* GetCentres() {return &mCentres[0];}
	void Extend(unsigned int number);
	unsigned int GetNumber() {return mNumber;}
	unsigned int GetSeed() {return mSeed;}

private:
	unsigned int mSeed;
	unsigned int mNumber;
	std::vector<DVector3> mCentres;
	DRandom mRandom;
};

class DStaticCentres
{
public:
	static DStaticCentres& Get();

	DVector3* GetCentres(unsigned int seed, unsigned int number);

private:
	std::vector<DCentreCollection> mCentreCollections;

};
