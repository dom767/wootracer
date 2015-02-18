#include "stdafx.h"
#include "PostProcess.h"
#include <math.h>
/*
class DThreadArgs
{
public:
	void Initialise(DParallelTask* parallelTask)
	{
		mParallelTask = parallelTask;mTaskIdentifier = GetNext();
	}
	void DoProcess()
	{
		mParallelTask->DoProcess(mTaskIdentifier);
	}
	bool GetNext()
	{
	// lock the critical section associated with parent while incrementing the row
		EnterCriticalSection(mParallelTask->GetCriticalSection());
		mTaskIdentifier = mParallelTask->GetNext();
		LeaveCriticalSection(mParallelTask->GetCriticalSection());
		if (mTaskIdentifier<0)
			return false;
		return true;
	}

private:
	DParallelTask* mParallelTask;
	int mTaskIdentifier;
};

DWORD WINAPI PostProcess_Thread(LPVOID arg)
{
	DThreadArgs* threadArgs = (DThreadArgs*)(arg);

	do
	{
		threadArgs->DoProcess();
	}
	while(threadArgs->GetNext());

	return 1;
}

int DPostProcess::GetNext()
{
	if (mRow<mWidth)
		return mRow++;
	else
		return -1;
}

void DPostProcess::DoProcess(int taskId)
{
	float* boostBuffer = new float[mWidth*3];

//    if (mBoostPower != 1)
    {
        double maxV = mMaxValue<0.001f ? 1 : mMaxValue;
        double boostP = mBoostPower;
        for (int i = 0; i < mWidth * 3; i++)
        {
            boostBuffer[i] = (float)pow(mSourceBuffer[(mWidth*taskId)+i] / maxV, boostP);
        }
    }
//    else
  //  {
    //    boostBuffer = mSourceBuffer;
   // }

    int patchStartX, patchEndX, patchStartY, patchEndY, x, y;
	float totalr, totalg, totalb, totalweighting;
	int px, py, bbidx;
	float kernelW;

		y = mRow; 
	for (int iter = 0; iter < mIterations; iter++)
    {
//        for (y = 0; y < height; y++)
        {
            for (x = 0; x < mWidth; x++)
            {
                patchStartX = max(0, x - 2) - x;
                patchEndX = min(x + 2, mWidth-1) - x;
                patchStartY = max(0, y - 2) - y;
                patchEndY = min(y + 2, mHeight-1) - y;

                // iterate over a patch
                totalr = totalg = totalb = totalweighting = 0;
				for (py = patchStartY; py <= patchEndY; py++)
                {
                    for (px = patchStartX; px <= patchEndX; px++)
                    {
                        kernelW = mKernel[(py+2) * 5 + px+2];
                        bbidx = ((py + y) * mWidth + (px + x)) * 3;
						totalr += boostBuffer[bbidx++] * kernelW;
                        totalg += boostBuffer[bbidx++] * kernelW;
                        totalb += boostBuffer[bbidx] * kernelW;
                        totalweighting += kernelW;
                    }
                }

                // divide through
                mTargetBuffer[(x + y * mWidth) * 3] = totalr / totalweighting;
                mTargetBuffer[(x + y * mWidth) * 3 + 1] = totalg / totalweighting;
                mTargetBuffer[(x + y * mWidth) * 3 + 2] = totalb / totalweighting;
            }
        }
		memcpy(boostBuffer, mTargetBuffer, sizeof(float)*3*mWidth/**mHeight*//*);
/*        for (int i = 0; i < width * height * 3; i++)
        {
            boostBuffer[i] = targetBuffer[i];
        }
*//*    }

    // divide through
    for (int x = 0; x < mWidth; x++)
    {
        mTargetBuffer[(x + y * mWidth) * 3] = mTargetWeighting * mTargetBuffer[(x + y * mWidth) * 3] + mSourceWeighting * mSourceBuffer[(x + y * mWidth) * 3];
        mTargetBuffer[(x + y * mWidth) * 3 + 1] = mTargetWeighting * mTargetBuffer[(x + y * mWidth) * 3 + 1] + mSourceWeighting * mSourceBuffer[(x + y * mWidth) * 3 + 1];
        mTargetBuffer[(x + y * mWidth) * 3 + 2] = mTargetWeighting * mTargetBuffer[(x + y * mWidth) * 3 + 2] + mSourceWeighting * mSourceBuffer[(x + y * mWidth) * 3 + 2];
    }

	delete [] boostBuffer;
}

void DParallelTaskManager::Add(DParallelTask* task)
{
	mTasks.push_back(task);
}

void DParallelTaskManager::Process()
{
}
*/
void DPostProcess::PostProcess(float* targetBuffer, float* sourceBuffer, double maxValue, int iterations, float* kernel, float boostPower, float targetWeighting, float sourceWeighting, int width, int height)
{
	float* boostBuffer = new float[width*height*3];

    double maxV = maxValue<0.001f ? 1 : maxValue;
    double boostP = boostPower;
    for (int i = 0; i < width * height * 3; i++)
    {
        boostBuffer[i] = (float)pow(sourceBuffer[i] / maxV, boostP);
    }

    int patchStartX, patchEndX, patchStartY, patchEndY, x, y;
	float totalr, totalg, totalb, totalweighting;
	int px, py, bbidx;
	float kernelW;

	for (int iter = 0; iter < iterations; iter++)
    {
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                patchStartX = max(0, x - 2) - x;
                patchEndX = min(x + 2, width-1) - x;
                patchStartY = max(0, y - 2) - y;
                patchEndY = min(y + 2, height-1) - y;

                // iterate over a patch
                totalr = totalg = totalb = totalweighting = 0;
				for (py = patchStartY; py <= patchEndY; py++)
                {
                    for (px = patchStartX; px <= patchEndX; px++)
                    {
                        kernelW = kernel[(py+2) * 5 + px+2];
                        bbidx = ((py + y) * width + (px + x)) * 3;
						totalr += boostBuffer[bbidx++] * kernelW;
                        totalg += boostBuffer[bbidx++] * kernelW;
                        totalb += boostBuffer[bbidx] * kernelW;
                        totalweighting += kernelW;
                    }
                }

                // divide through
                targetBuffer[(x + y * width) * 3] = totalr / totalweighting;
                targetBuffer[(x + y * width) * 3 + 1] = totalg / totalweighting;
                targetBuffer[(x + y * width) * 3 + 2] = totalb / totalweighting;
            }
        }
		memcpy(boostBuffer, targetBuffer, sizeof(float)*3*width*height);
    }

    // divide through
    for (int i = 0; i < height * width * 3; i++)
    {
        targetBuffer[i] = targetWeighting * targetBuffer[i] + sourceWeighting * sourceBuffer[i];
    }

	delete [] boostBuffer;

	
	
	
	
	
	/*	// non-reentrant to simplify the code!!
#ifdef _DEBUG
	const int NumThreads = 1;
#else
	const int NumThreads = 4;
#endif

	DParallelTaskManager taskManager;
	for (int i=0; i<NumThreads; i++)
	{
		taskManager.Add(new DPostProcessWorker(targetBuffer, sourceBuffer, maxValue, iterations, kernel, boostPower, targetWeighting, sourceWeighting, width, height, (height*i)/NumThreads, (height*(i+1))/NumThreads);
	}

	for (int i=0; i<iterations; i++)
	{
		taskManager.Process();
	}

	HANDLE threadHandle[NumThreads];
	DWORD threadId[NumThreads];
	DThreadArgs args[NumThreads];

	for (int i=0; i<NumThreads; i++)
	{
		args[i].Initialise(this);
		threadHandle[i] = CreateThread(NULL, 0, PostProcess_Thread, &(args[i]), 0, &threadId[i]);
	}

	int finishedThread = WaitForMultipleObjects(NumThreads, threadHandle, true, INFINITE);
*/
}
