#include "stdafx.h"
#include "log.h"

DLog& Log()
{
	static DLog log;
	return log;
}

DLog::DLog()
{
	fopen_s(&mFile, "log.txt", "wb");
}

DLog::~DLog()
{
	fclose(mFile);
}

void DLog::Print(const char* string)
{
	fputs(string, mFile);
	fputs("\r\n", mFile);
}