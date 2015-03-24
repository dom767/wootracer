#pragma once
#include <vector>
/*
class DParallelTaskManager
{
public:
	void Add(DParallelTask* task);
	void Process();

private:
	std::vector<DParallelTask*> mTasks;

};

class DParallelTask
{
public:
	virtual void Process()=0;

protected:

};
*//*
class DPostProcessTask : public DParallelTask
{
public:
	DPostProcessTask(float* targetBuffer, float* sourceBuffer, double maxValue, int iterations, float* kernel, float boostPower, float targetweighting, float sourceweighting, int width, int height, int startRow, int endRow);

	void Process();

private:
	float* mTargetBuffer;
	float* mSourceBuffer;
	double mMaxValue;
	int mIterations;
	float* mKernel;
	float mBoostPower;
	float mTargetWeighting;
	float mSourceWeighting;
	int mWidth;
	int mHeight;
	int mStartRow;
	int mEndRow;

};
*/
class DPostProcess
{
public:
	void PostProcess(float* targetBuffer, float* sourceBuffer, double maxValue, int iterations, float* kernel, float boostPower, float targetweighting, float sourceweighting, int width, int height);

	void GaussianBlur(float* targetBuffer, float* sourceBuffer, double maxValue, int size, float boostPower, float targetweighting, float sourceweighting, int width, int height);

};
