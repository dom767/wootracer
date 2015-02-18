#pragma once

class DTimer
{
public:
	void StartTimer();
	double EndTimer();

private:
	__int64 mTimerStart;
	__int64 mTimerEnd;

};
