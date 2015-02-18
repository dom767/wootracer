#include "stdafx.h"
#include "Timer.h"

void DTimer::StartTimer()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&mTimerStart);
}

double DTimer::EndTimer()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&mTimerEnd);
	__int64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	return double(mTimerEnd-mTimerStart)/freq;
}
