#pragma once

#include <string>
#include <vector>
#include "Vector3.h"
#include "Colour.h"

class DVector3;
class DFuncParam;
class DFunctionState;
#define BEGIN_VECFUNC(name, desc) class name : public DVecFunc \
{ \
public: \
	virtual DVecFunc* Clone() \
	{ \
	return new name(); \
	} \
	virtual std::string GetName() \
	{ \
	return std::string(desc); \
	}

#define END_VECFUNC };

#define BEGIN_NULLFUNC(name, desc) class name : public DNullFunc \
{ \
public: \
	virtual DNullFunc* Clone() \
	{ \
	return new name(); \
	} \
	virtual std::string GetName() \
	{ \
	return std::string(desc); \
	}

#define END_NULLFUNC };

#define BEGIN_FUNC(name, desc) class name : public DFloatFunc \
{ \
public: \
	virtual DFloatFunc* Clone() \
	{ \
	return new name(); \
	} \
	virtual std::string GetName() \
	{ \
	return std::string(desc); \
	}

#define END_FUNC };

enum EType
{
	Float = 1,
	Vec = 2,
	Null = 3, // function only
	RawFloat = 4
};

class DDistFunc;
class DNullFunc;

enum EFloatStateVar
{
	Distance=0,
	LastFloatStateVar
};

enum EVectorStateVar
{
	Position=0,
	LastVectorStateVar
};

class DProgram
{
public:
	~DProgram() {Reset();}

	void Create(std::string& program);
	void Run(DFunctionState& state) const;
	bool Compiled() const {return mCompiled;}
	void Reset();

private:
	std::vector<DNullFunc*> mStatements;
	bool mCompiled;

};

class DFunctionState
{
public:
	float mDistance; // return type
	DVector3 mPosition;
	DVector3 mObjectPosition;
	DColour mDiffuse;
	DColour mSpecular;
	DColour mReflectivity;
	DColour mEmissive;
	DColour mAbsorption;
	DVector3 mTrapPosition;
};

class DFuncParam
{
public:
	DFuncParam(char* name, EType type)
	{
		mVar = NULL;
		mName = name;
		mType = type;
	}
	~DFuncParam();

	float Evaluate(DFunctionState& state);
	DVector3 EvaluateVec(DFunctionState& state);
	void EvaluateNull(DFunctionState& state);

	DDistFunc* mVar;
	char* mName;
	EType mType;
	DVector3 mCacheVector3;
	float mCacheFloat;
};

class DDistFunc
{
public:
	DDistFunc() : mConstant(true), mSet(false) {}
	virtual ~DDistFunc();
	virtual void Read(std::string& arg);
	virtual EType GetType()=0;
	virtual std::string GetName()=0;
	virtual DDistFunc* Clone()=0;

	void SetParams(std::vector<DDistFunc*> params)
	{
		for (unsigned int i=0; i<params.size(); i++)
		{
			mParam[i]->mVar = params[i];
			if (!params[i]->mConstant)
				mConstant = false;
		}
	}

	std::vector<DFuncParam*> mParam;
	bool mConstant;
	bool mSet;
};

class DVecFunc : public DDistFunc
{
public:
	virtual DVector3 Evaluate(DFunctionState& state)=0;
//	virtual void Read(std::string& arg)=0;
	virtual EType GetType() {return Vec;}
	virtual DVecFunc* Clone()=0;
	virtual std::string GetName()=0;
};

class DFloatFunc : public DDistFunc
{
public:
	virtual float Evaluate(DFunctionState& state)=0;
//	virtual void Read(std::string& arg)=0;
	virtual EType GetType() {return Float;}
	virtual DFloatFunc* Clone()=0;
	virtual std::string GetName()=0;
};

class DNullFunc : public DDistFunc
{
public:
	virtual void Evaluate(DFunctionState& state)=0;
//	virtual void Read(std::string& arg)=0;
	virtual EType GetType() {return Null;}
	virtual DNullFunc* Clone()=0;
	virtual std::string GetName()=0;
};
