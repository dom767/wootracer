#include "stdafx.h"
#include "convert.h"
#include "Vector3.h"
#include "Colour.h"
#include "Matrix4.h"
#include <stdlib.h>
#include <string>
#include <sstream>

void Convert::StrToBool(const char* str, bool& value)
{
	if (!str)
		return;

	value = strcmp(str, "true") == 0;
}

void Convert::StrToInt(const char* str, int& value)
{
	if (!str)
		return;

	value = atoi(str);
}

void Convert::StrToVec3(const char* str, DVector3& vector)
{
	if (!str)
		return;

	std::stringstream ss(str);
    std::string item;

	std::getline(ss, item, ',');
	StrToFloat(item.c_str(), vector.mComponent[0]);

	std::getline(ss, item, ',');
	StrToFloat(item.c_str(), vector.mComponent[1]);

	std::getline(ss, item, ',');
	StrToFloat(item.c_str(), vector.mComponent[2]);
}

void Convert::StrToMat3(const char* str, DMatrix4& matrix)
{
	if (!str)
		return;

	std::stringstream ss(str);
    std::string item;

	matrix.MakeIdentity();
	std::getline(ss, item, ',');
	StrToFloat(item.c_str(), matrix.mVal[0][0]);

	std::getline(ss, item, ',');
	StrToFloat(item.c_str(), matrix.mVal[0][1]);

	std::getline(ss, item, ',');
	StrToFloat(item.c_str(), matrix.mVal[0][2]);

	std::getline(ss, item, ',');
	StrToFloat(item.c_str(), matrix.mVal[1][0]);

	std::getline(ss, item, ',');
	StrToFloat(item.c_str(), matrix.mVal[1][1]);

	std::getline(ss, item, ',');
	StrToFloat(item.c_str(), matrix.mVal[1][2]);

	std::getline(ss, item, ',');
	StrToFloat(item.c_str(), matrix.mVal[2][0]);

	std::getline(ss, item, ',');
	StrToFloat(item.c_str(), matrix.mVal[2][1]);

	std::getline(ss, item, ',');
	StrToFloat(item.c_str(), matrix.mVal[2][2]);
}

void Convert::StrToFloat(const char* str, float& value)
{
	if (!str)
		return;

	value = (float)strtod(str, NULL);
}

void Convert::StrToCol(const char* str, DColour& colour)
{
	if (!str)
		return;

	std::stringstream ss(str);
    std::string item;

	std::getline(ss, item, ',');
	StrToFloat(item.c_str(), colour.mRed);

	std::getline(ss, item, ',');
	StrToFloat(item.c_str(), colour.mGreen);

	std::getline(ss, item, ',');
	StrToFloat(item.c_str(), colour.mBlue);
}
