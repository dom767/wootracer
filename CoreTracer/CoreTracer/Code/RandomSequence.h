#pragma once

#include "Vector2.h"
#include "Random.h"
#include <vector>

struct DSingleSequence
{
	DVector2 mValues[256];
};

class DRandomSequence
{
public:
	enum EGenerator
	{
		Random,
		MostDistant2,
		MostDistant4,
		Jitter,
		Poisson,
		PoissonJitter,
		HeirarchicalJitter,
		GridJitter,
		IterativeJitter,
		IterativePoissonJitter,
		RelaxingPoisson
	};

	DRandomSequence(EGenerator generator, int totalSequences, int seed) : mRandom(seed)
	{
		mGenerator = generator;
		mTotalSequences = totalSequences;
		mStartSeed = seed;
	}

	void GenerateSequences();

	DVector2 GetValue(int sequence, int index) const
	{
		return mSequences[sequence].mValues[index];
	}

private:
	void GenerateSequence(DSingleSequence& sequence);
	void WriteSequences(FILE *file);
	void ReadSequences(FILE *file);

	EGenerator mGenerator;
	int mTotalSequences;
	std::vector<DSingleSequence> mSequences;
	DRandom mRandom;
	int mStartSeed;

};