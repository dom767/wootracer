#include "stdafx.h"
#include "StaticCentres.h"

DStaticCentres& DStaticCentres::Get()
{
	static DStaticCentres singleton;
	return singleton;
}

DVector3* DStaticCentres::GetCentres(unsigned int seed, unsigned int number)
{
	for (unsigned i=0; i<mCentreCollections.size(); i++)
	{
		if (seed==mCentreCollections[i].GetSeed())
		{
			if (number<=mCentreCollections[i].GetNumber())
			{
				return mCentreCollections[i].GetCentres();
			}
			else
			{
				mCentreCollections[i].Extend(number);
				return mCentreCollections[i].GetCentres();
			}
		}
	}

	DCentreCollection newCollection(seed);
	newCollection.Extend(number);
	mCentreCollections.push_back(newCollection);

	return mCentreCollections[mCentreCollections.size()-1].GetCentres();
}

DCentreCollection::DCentreCollection(unsigned int seed) : mRandom(seed), mSeed(seed), mNumber(0)
{
}

void DCentreCollection::Extend(unsigned int number)
{
	for (unsigned i=0; i<number-mNumber; i++)
	{
		mCentres.push_back(DVector3(2*mRandom.GetNextFloat()-1, 2*mRandom.GetNextFloat()-1, 2*mRandom.GetNextFloat()-1));
	}
	mNumber = number;
}