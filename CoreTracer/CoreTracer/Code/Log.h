#pragma once

#include <stdio.h>

#define LOG(errorLevel, msg) if(Log().mErrorLevel<=errorLevel) Log().Print(msg);

enum ErrorLevel
{
	Info=0,
	Error
};

class DLog;

DLog& Log();

class DLog
{
public:
	DLog();
	~DLog();

	void Print(const char* string);
	void SetErrorLevel(ErrorLevel errorLevel) {mErrorLevel=errorLevel;}

	ErrorLevel mErrorLevel;

private:
	FILE* mFile;

};