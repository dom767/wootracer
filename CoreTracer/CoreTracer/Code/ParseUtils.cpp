#include "stdafx.h"
#include "ParseUtils.h"
#include "Vector3.h"

int ParseUtils::GetIndex(const std::string &arg, char findchar)
{
	int nextspace = arg.find(findchar);
	if (nextspace==-1) nextspace = arg.length();
	return nextspace;
}

std::string ParseUtils::GetToken(std::string& arg)
{
	while(arg[0]==' ') arg = arg.substr(1, arg.length());
		
	int nextspace = GetIndex(arg, ' ');
	int nextopenbracket = GetIndex(arg, '(');
	int nextclosebracket = GetIndex(arg, ')');
	int nextcomma = GetIndex(arg, ',');
	int idx = min(nextspace, min(nextopenbracket, min(nextclosebracket, nextcomma)));
		
	if (idx==0) idx=1;
	std::string ret = arg.substr(0, idx);
	arg = arg.substr(idx, arg.length());
	return ret;
}

float ParseUtils::IsFloat(std::string& arg)
{
	int dots = 0;
	for (unsigned int i=0; i<arg.length(); i++)
	{
		if (i==0 && arg[0]=='-')
		{
			if (arg.length()==1) return false;
		}
		else
		{
			if (arg[i]=='.') dots++;
			else if (arg[i]<'0' || arg[i]>'9') return false;
			if (dots>1) return false;
		}
	}
	return true;
}

float ParseUtils::GetFloat(std::string& arg)
{
	std::string token = ParseUtils::GetToken(arg);
	return (float)::atof(token.c_str());
}

bool ParseUtils::IsOpenBracket(const std::string& arg)
{
	std::string arg2 = arg;
	std::string token = ParseUtils::GetToken(arg2);
	if (token.compare("(")!=0)
		return false;
	else
		return true;
}

bool ParseUtils::IsCloseBracket(const std::string& arg)
{
	std::string arg2 = arg;
	std::string token = ParseUtils::GetToken(arg2);
	if (token.compare(")")!=0)
		return false;
	else
		return true;
}

bool ParseUtils::IsComma(const std::string& arg)
{
	std::string arg2 = arg;
	std::string token = ParseUtils::GetToken(arg2);
	if (token.compare(",")!=0)
		return false;
	else
		return true;
}

void ParseUtils::StripOpenBracket(std::string& arg)
{
	std::string token = ParseUtils::GetToken(arg);
	if (token.compare("(")!=0)
		throw 0x1;
}

void ParseUtils::StripCloseBracket(std::string& arg)
{
	std::string token = ParseUtils::GetToken(arg);
	if (token.compare(")")!=0)
		throw 0x1;
}

void ParseUtils::StripComma(std::string& arg)
{
	std::string token = ParseUtils::GetToken(arg);
	if (token.compare(",")!=0)
		throw 0x1;
}

DVector3 ParseUtils::GetVector3(std::string& arg)
{
	DVector3 ret;

	std::string token = ParseUtils::GetToken(arg);
	if (token.compare("vec")!=0)
		throw 0x1;
		
	ParseUtils::StripOpenBracket(arg);
	ret.mComponent[0] = ParseUtils::GetFloat(arg);
	ParseUtils::StripComma(arg);
	ret.mComponent[1] = ParseUtils::GetFloat(arg);
	ParseUtils::StripComma(arg);
	ret.mComponent[2] = ParseUtils::GetFloat(arg);
	ParseUtils::StripCloseBracket(arg);
	return ret;
}
