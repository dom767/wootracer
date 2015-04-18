#pragma once

#include <vector>
#include <string>
#include "Vector3.h"

class DFloatFunc;
class DVecFunc;
class DNullFunc;
class DDistFunc;
class DFuncParam;
class DFunctionState;
enum EType;
enum EFloatStateVar;
enum EVectorStateVar;

enum EVariableName
{
	VNPosition,
	VNObjectPosition,
	VNDistance,
	VNDiffuse,
	VNSpecular,
	VNReflectivity,
	VNEmissive,
	VNAbsorption,
	END_VARIABLE_NAME
};

class DVariable
{
public:
	DVariable(EType type, EVariableName name, const char* description) : mType(type), mName(name), mDescription(description) {}

	EType GetType() const {return mType;}
	EVariableName GetName() const {return mName;}
	std::string GetDescription() const {return mDescription;}

private:
	EType mType;
	EVariableName mName;
	std::string mDescription;
};

class DFuncFactory
{
public:
	static DFuncFactory& Get()
	{
		static DFuncFactory factory;
		return factory;
	}
	DFuncFactory();
	~DFuncFactory();

	void BuildSchema();

	int GetSchemaLength() {return mSchema.length();}
	const char* GetSchema() {return mSchema.c_str();}

	std::vector<DNullFunc*> Create(std::string& arg);
	DDistFunc* CreateFunction(std::string& arg);

	float GetFloatValue(DFunctionState& state, DVariable* var);
	DVector3 GetVectorValue(DFunctionState& state, DVariable* var);
	void SetFloatValue(DFunctionState& state, DVariable* var, float val);
	void SetVectorValue(DFunctionState& state, DVariable* var, DVector3& val);

	DVariable* GetVariable(std::string& arg);

private:
	std::string WriteParams(std::vector<DFuncParam*> params);
	std::string GetParamType(EType type);

	std::vector<DVariable*> mVariableList;
	std::vector<DDistFunc*> mFuncList;
	std::string mSchema;

	bool IsFloatVar(std::string& token);
	bool IsVectorVar(std::string& token);
};
