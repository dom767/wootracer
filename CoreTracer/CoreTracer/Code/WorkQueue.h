#pragma once

#include <list>

class DRenderFragment;

class DWorkQueue
{
public:
	DWorkQueue();
	~DWorkQueue();

	void Add(DRenderFragment* fragment);
	void Update(DRenderFragment* fragment);
	void SetStop();
	DRenderFragment* GetHead();

private:
	CRITICAL_SECTION mWQCS;
	std::list<DRenderFragment*> mFragments;
	bool mStop;

};
