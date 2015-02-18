#include "stdafx.h"
#include "RandomSequence.h"

void DRandomSequence::GenerateSequences()
{
	for (int i=0; i<mTotalSequences; i++)
	{
		DSingleSequence sequence;
		GenerateSequence(sequence);
		mSequences.push_back(sequence);
	}
}

int GetFurthest(DVector2* samples, int pointIdx, DVector2* randVals, int numRandVals)
{
	int maxMinIdx = 0;
	float maxMinDist = 0;

	for (int v=0; v<numRandVals; v++)
	{
		float minDist = 10000;
		for (int p=0; p<pointIdx; p++)
		{
			float dx = fabsf(randVals[v].x - samples[p].x);
			if (dx>0.5) dx = 1-dx;
			float dy = fabsf(randVals[v].y - samples[p].y);
			if (dy>0.5) dy = 1-dy;

			float dist = sqrtf(dx*dx + dy*dy);
			if (dist<minDist) minDist = dist;
		}

		if (minDist>maxMinDist)
		{
			maxMinDist = minDist;
			maxMinIdx = v;
		}
	}

	return maxMinIdx;
}

bool within(DVector2* samples, int sampleSize, float randx, float randy, float tolerance)
{
	for (int p=0; p<sampleSize; p++)
	{
		float dx = fabsf(samples[p].x - randx);
		if (dx>0.5) dx = 1-dx;
		float dy = fabsf(samples[p].y - randy);
		if (dy>0.5) dy = 1-dy;

		float dist = sqrtf(dx*dx + dy*dy);
		if (dist<tolerance) return true;
	}
	return false;
}

class DGridOfSamples
{
public:
	DGridOfSamples(DVector2 samplePosition, DVector2 topLeft, DVector2 extents, DRandom* random) :
		mSamplePosition(samplePosition),
		mTopLeft(topLeft),
		mExtents(extents),
		mSubSampled(false),
		mRandom(random)
	{
		mChildren[0]=mChildren[1]=mChildren[2]=mChildren[3]=NULL;
	}

	void GetLeaves(std::vector<DGridOfSamples*> &leaves)
	{
		if (mSubSampled)
		{
			for (int i=0; i<4; i++)
				mChildren[i]->GetLeaves(leaves);
		}
		else
		{
			leaves.push_back(this);
		}
	}

	void SubSample(DVector2* values, int* idx)
	{
		if (mSubSampled)
			return;

		DVector2 middle = mTopLeft + mExtents*0.5f;
		DVector2 newTopLeft;
		int currentIndex = (mSamplePosition.x < middle.x ? 0 : 1) + (mSamplePosition.y < middle.y ? 0 : 2);
		newTopLeft.x = currentIndex%2==0 ? mTopLeft.x : middle.x;
		newTopLeft.y = currentIndex/2==0 ? mTopLeft.y : middle.y;
		mChildren[currentIndex] = new DGridOfSamples(mSamplePosition, newTopLeft, mExtents*0.5, mRandom);

		for (int i=0; i<3; i++)
		{
			int newIdx;
			DVector2 randomSample;
			do
			{
				randomSample = DVector2(mTopLeft.x + mExtents.x*mRandom->GetNextFloat(),
					mTopLeft.y + mExtents.y*mRandom->GetNextFloat());
				newIdx = (randomSample.x < middle.x ? 0 : 1) + (randomSample.y < middle.y ? 0 : 2);
			}
			while(mChildren[newIdx]!=NULL);

			values[(*idx)++] = randomSample;

			newTopLeft.x = newIdx%2==0 ? mTopLeft.x : middle.x;
			newTopLeft.y = newIdx/2==0 ? mTopLeft.y : middle.y;
			mChildren[newIdx] = new DGridOfSamples(randomSample, newTopLeft, mExtents*0.5, mRandom);
		}

		mSubSampled = true;
	}

private:
	DGridOfSamples* mChildren[4];
	bool mSubSampled;
	DVector2 mSamplePosition;
	DVector2 mTopLeft;
	DVector2 mExtents;
	DRandom* mRandom;
};

void Shuffle(std::vector<DGridOfSamples*> &sampleGrids, DRandom* random)
{
	for (unsigned int i=0; i<sampleGrids.size(); i++)
	{
		DGridOfSamples* tmp = sampleGrids[i];
		int randomIdx = int( random->GetNextFloat()*sampleGrids.size());
		sampleGrids[i] = sampleGrids[randomIdx];
		sampleGrids[randomIdx] = tmp;
	}
}

void DRandomSequence::GenerateSequence(DSingleSequence& sequence)
{
	switch(mGenerator)
	{
	case Random:
		for (int i=0; i<256; i++)
		{
			sequence.mValues[i].x = mRandom.GetNextFloat();
			sequence.mValues[i].y = mRandom.GetNextFloat();
		}
		break;
	case MostDistant2:
		for (int i=0; i<256; i++)
		{
			int randVals = 2;
			DVector2* randVal = new DVector2[2];

			for (int r=0; r<randVals; r++)
			{
				randVal[r].x = mRandom.GetNextFloat();
				randVal[r].y = mRandom.GetNextFloat();
			}

			int furthest = GetFurthest(sequence.mValues, i, randVal, randVals);

			sequence.mValues[i].x = randVal[furthest].x;
			sequence.mValues[i].y = randVal[furthest].y;

			delete [] randVal;
		}
		break;
	case MostDistant4:
		for (int i=0; i<256; i++)
		{
			int randVals = 4;
			DVector2* randVal = new DVector2[4];

			for (int r=0; r<randVals; r++)
			{
				randVal[r].x = mRandom.GetNextFloat();
				randVal[r].y = mRandom.GetNextFloat();
			}

			int furthest = GetFurthest(sequence.mValues, i, randVal, randVals);

			sequence.mValues[i].x = randVal[furthest].x;
			sequence.mValues[i].y = randVal[furthest].y;

			delete [] randVal;
		}
		break;
	case Jitter:
		int points[256];
		for (int i=0; i<256; i++)
			points[i]=0;
		for (int i=0; i<256; i++)
		{
			int idx;
			float randx;
			float randy;
			do
			{
				randx = mRandom.GetNextFloat();
				randy = mRandom.GetNextFloat();
				idx = int(15.99f*randx) + 16*int(15.99f*randy);
//				if (idx>255)
			}
			while (points[idx]==1);
			points[idx]=1;
			sequence.mValues[i] = DVector2(randx, randy);
		}
		break;
	case Poisson:
		for (int i=0; i<256; i++)
		{
			float randx, randy;
			do
			{
				randx = mRandom.GetNextFloat();
				randy = mRandom.GetNextFloat();
			}
			while (within(sequence.mValues, i, randx, randy, 0.045f));

			sequence.mValues[i] = DVector2(randx, randy);
		}
		break;		
	case PoissonJitter:
		for (int i=0; i<256; i++)
			points[i]=0;
		for (int i=0; i<256; i++)
		{
			float randx, randy;
			int idx;
			do
			{
				randx = mRandom.GetNextFloat();
				randy = mRandom.GetNextFloat();
				idx = int(15.99f*randx) + 16*int(15.99f*randy);
			}
			while (points[idx]==1 || within(sequence.mValues, i, randx, randy, 0.028f));
			points[idx] = 1;

			sequence.mValues[i] = DVector2(randx, randy);
		}
		break;
	case HeirarchicalJitter:
		int points1[4];
		int points2[16];
		int points3[64];
		int points4[256];

		for (int i=0; i<4; i++)
			points1[i]=0;
		for (int i=0; i<16; i++)
			points2[i]=0;
		for (int i=0; i<64; i++)
			points3[i]=0;
		for (int i=0; i<256; i++)
			points4[i]=0;

		for (int i=0; i<256; i++)
		{
			int idx1, idx2, idx3, idx4;
			float randx;
			float randy;
			do
			{
				randx = mRandom.GetNextFloat();
				randy = mRandom.GetNextFloat();
				idx1 = int(1.99f*randx) + 2*int(1.99f*randy);
				idx2 = int(3.99f*randx) + 4*int(3.99f*randy);
				idx3 = int(7.99f*randx) + 8*int(7.99f*randy);
				idx4 = int(15.99f*randx) + 16*int(15.99f*randy);
			}
			while ((i<4 && points1[idx1]==1)
				|| (i>=4 && i<16 && points2[idx2]==1)
				|| (i>=16 && i<64 && points3[idx3]==1)
				|| (i>=64 && i<256 && points4[idx4]==1));
			points1[idx1]=1;
			points2[idx2]=1;
			points3[idx3]=1;
			points4[idx4]=1;
			sequence.mValues[i] = DVector2(randx, randy);
		}
		break;
	case GridJitter:
		DVector2 sample(mRandom.GetNextFloat(), mRandom.GetNextFloat());
		DGridOfSamples sampleGrid(sample, DVector2(0,0), DVector2(1,1), &mRandom);

		int idx=0;
		sequence.mValues[idx++] = sample;
		sampleGrid.SubSample(sequence.mValues, &idx);

		std::vector<DGridOfSamples*> sampleGrids;

		for (int g=0; g<3; g++)
		{
			sampleGrids.clear();
			sampleGrid.GetLeaves(sampleGrids);
		
			Shuffle(sampleGrids, &mRandom);
			for (unsigned int i=0; i<sampleGrids.size(); i++)
			{
				sampleGrids[i]->SubSample(sequence.mValues, &idx);
			}
		}
		break;
	}
}