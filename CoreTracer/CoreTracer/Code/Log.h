#pragma once

#include <stdio.h>

class DLog;

DLog& Log();

class DLog
{
public:
	DLog();
	~DLog();

	void Print(const char* string);

private:
	FILE* mFile;
};