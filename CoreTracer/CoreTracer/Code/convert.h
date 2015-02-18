#pragma once

class DVector3;
class DColour;
class DMatrix4;

namespace Convert
{
	void StrToBool(const char* str, bool& value);
	void StrToInt(const char* str, int& value);
	void StrToMat3(const char* str, DMatrix4& matrix);
	void StrToVec3(const char* str, DVector3& vector);
	void StrToFloat(const char* str, float& value);
	void StrToCol(const char* str, DColour& colour);
};
