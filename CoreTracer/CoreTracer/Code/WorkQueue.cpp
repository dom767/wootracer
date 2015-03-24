#include "stdafx.h"
#include "WorkQueue.h"
#include "RenderFragment.h"
#include <windows.h>


DWorkQueue::DWorkQueue() : mStop(false)
{
	InitializeCriticalSection(&mWQCS);
}

DWorkQueue::~DWorkQueue()
{
	while (mFragments.size()>0)
	{
		delete mFragments.front();
		mFragments.pop_front();
	}
}

void DWorkQueue::Add(DRenderFragment* fragment)
{
	// lock shouldn't be necessary here, but WTF
	EnterCriticalSection(&mWQCS);
	mFragments.push_back(fragment);
	LeaveCriticalSection(&mWQCS);
}

void DWorkQueue::Update(DRenderFragment* fragment)
{
	EnterCriticalSection(&mWQCS);
	mFragments.push_back(fragment);
	LeaveCriticalSection(&mWQCS);
}

DRenderFragment* DWorkQueue::GetHead()
{
	if (mStop)
		return 0;

	EnterCriticalSection(&mWQCS);
	DRenderFragment* ret = NULL;
	
	if (mFragments.size()>0)
	{
		ret = mFragments.front();
		mFragments.remove(ret);
	}

	LeaveCriticalSection(&mWQCS);
	return ret;
}

void DWorkQueue::SetStop()
{
	mStop = true;
}