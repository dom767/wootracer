#pragma once
#include <string>

class DVector3;

class ParseUtils
{
public:
	static int GetIndex(const std::string &arg, char findchar);
	static std::string GetToken(std::string& arg);
	static float IsFloat(std::string& arg);
	static float GetFloat(std::string& arg);
	static bool IsOpenBracket(const std::string& arg);
	static bool IsCloseBracket(const std::string& arg);
	static bool IsComma(const std::string& arg);
	static void StripOpenBracket(std::string& arg);
	static void StripCloseBracket(std::string& arg);
	static void StripComma(std::string& arg);
	static DVector3 GetVector3(std::string& arg);

};
