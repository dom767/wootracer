#include "stdafx.h"
#include "FuncFactory.h"
#include <string>
#include "Functions.h"
#include "ParseUtils.h"
#include "Log.h"
#include <sstream>

DFuncFactory::DFuncFactory()
{
	mFuncList.push_back(new DDistSphere());
	mFuncList.push_back(new DDistCapsule());
	mFuncList.push_back(new DDistCylinder());
//	mFuncList.push_back(new DDistTetra());	
	mFuncList.push_back(new DDistMin());
	mFuncList.push_back(new DDistSMin());
	mFuncList.push_back(new DDistSMin3());
	mFuncList.push_back(new DDistMax());
	mFuncList.push_back(new DDistNeg());
	mFuncList.push_back(new DDistMag());
//	mFuncList.push_back(new DDistFloat());
	mFuncList.push_back(new DDistMandelBox());
	mFuncList.push_back(new DDistMandelBulb());
	mFuncList.push_back(new DDistSignedBox());
	mFuncList.push_back(new DDistSignedTorus());
	mFuncList.push_back(new DDistGetX());
	mFuncList.push_back(new DDistGetY());
	mFuncList.push_back(new DDistGetZ());
	mFuncList.push_back(new DDistKaleido());
	mFuncList.push_back(new DDistKaleidoCol());
	mFuncList.push_back(new DDistKaleidoTrap());
	mFuncList.push_back(new DDistKaleidoDETrap());
	mFuncList.push_back(new DAdd());
	mFuncList.push_back(new DDistSub());
	mFuncList.push_back(new DDistMul());
	mFuncList.push_back(new DDistDiv());
	mFuncList.push_back(new DRound());
	mFuncList.push_back(new DMod());
	mFuncList.push_back(new DAbs());
	mFuncList.push_back(new DMad());
	mFuncList.push_back(new DFloor());
	mFuncList.push_back(new DDistPerlinXYZ());
	mFuncList.push_back(new DDistTurb3d());
	mFuncList.push_back(new DDistVoronoi());
	mFuncList.push_back(new DDistWaves());

	mFuncList.push_back(new DDistPos());
	mFuncList.push_back(new DDistVec());
	mFuncList.push_back(new DDistRotX());
	mFuncList.push_back(new DDistRotY());
	mFuncList.push_back(new DDistRotZ());
	mFuncList.push_back(new DDistRep());
	mFuncList.push_back(new DDistRepX());
	mFuncList.push_back(new DDistRepY());
	mFuncList.push_back(new DDistRepZ());
	mFuncList.push_back(new DDistRepXYZ());
	mFuncList.push_back(new DDistCosX());
	mFuncList.push_back(new DDistPerlinX());
	mFuncList.push_back(new DDistPerlinXZ());
	mFuncList.push_back(new DDistFold());
	mFuncList.push_back(new DDistSingleFold());
	mFuncList.push_back(new DDistSingleFold3D());
	mFuncList.push_back(new DDistRotateFold());
	mFuncList.push_back(new DDistFoldX());
	mFuncList.push_back(new DDistBoxFold());
	mFuncList.push_back(new DDistSphereFold());
	mFuncList.push_back(new DDistOuterSphereFold());
	mFuncList.push_back(new DDistCylinderFold());
	mFuncList.push_back(new DAddVec());
	mFuncList.push_back(new DDistSubVec());
	mFuncList.push_back(new DDistMulVec());
	mFuncList.push_back(new DDistMulVecF());
	mFuncList.push_back(new DDistDivVec());
	mFuncList.push_back(new DLerp());
	mFuncList.push_back(new DDistNormalise());
	mFuncList.push_back(new DSqrt());
	mFuncList.push_back(new DCos());
	mFuncList.push_back(new DSin());
	mFuncList.push_back(new DTan());
	mFuncList.push_back(new DPow());

	mFuncList.push_back(new DSet());
	mFuncList.push_back(new DSetV());
	mFuncList.push_back(new DDistRepeat());

	mVariableList.push_back(new DVariable(Float, VNDistance, "distance"));
	mVariableList.push_back(new DVariable(Vec, VNPosition, "wpos"));
	mVariableList.push_back(new DVariable(Vec, VNObjectPosition, "pos"));
	mVariableList.push_back(new DVariable(Vec, VNDiffuse, "diff"));
	mVariableList.push_back(new DVariable(Vec, VNSpecular, "spec"));
	mVariableList.push_back(new DVariable(Vec, VNReflectivity, "refl"));
	mVariableList.push_back(new DVariable(Vec, VNEmissive, "emi"));
	mVariableList.push_back(new DVariable(Vec, VNAbsorption, "absorb"));
	mVariableList.push_back(new DVariable(Vec, VNTrapPosition, "trappos"));

	BuildSchema();
}

DFuncFactory::~DFuncFactory()
{
	for (unsigned int i=0; i<mFuncList.size(); i++)
	{
		delete mFuncList[i];
	}
	for (unsigned int i=0; i<mVariableList.size(); i++)
	{
		delete mVariableList[i];
	}
}

float DFuncFactory::GetFloatValue(DFunctionState& state, DVariable* var)
{
	if (var->GetType() == Vec)
	{
		LOG(Error, "Wrong variable type to GetFloatValue");
		throw 1;
	}

	switch(var->GetName())
	{
	case VNDistance:
		return state.mDistance;
	}

	LOG(Error, "Missing variable in GetFloatValue");
	throw;
}

DVector3 DFuncFactory::GetVectorValue(DFunctionState& state, DVariable* var)
{
	if (var->GetType() == Float)
	{
		LOG(Error, "Wrong variable type to GetVectorValue");
		throw;
	}

	switch(var->GetName())
	{
	case VNPosition:
		return state.mPosition;
	case VNObjectPosition:
		return state.mObjectPosition;
	case VNDiffuse:
		return state.mDiffuse.GetVector();
	case VNSpecular:
		return state.mSpecular.GetVector();
	case VNReflectivity:
		return state.mReflectivity.GetVector();
	case VNEmissive:
		return state.mEmissive.GetVector();
	case VNAbsorption:
		return state.mAbsorption.GetVector();
	case VNTrapPosition:
		return state.mTrapPosition;
	}

	LOG(Error, "Missing variable in GetVectorValue");
	throw;
}

void DFuncFactory::SetFloatValue(DFunctionState& state, DVariable* var, float val)
{
	if (var->GetType() == Vec)
	{
		LOG(Error, "Wrong variable type to SetFloatValue");
		throw;
	}

	switch(var->GetName())
	{
	case VNDistance:
		state.mDistance = val;
		return;
	}

	LOG(Error, "Missing variable in SetFloatValue");
	throw;
}

void DFuncFactory::SetVectorValue(DFunctionState& state, DVariable* var, DVector3& val)
{
	if (var->GetType() == Float)
	{
		LOG(Error, "Wrong variable type to SetVectorValue");
		throw;
	}

	switch(var->GetName())
	{
	case VNPosition:
		state.mPosition = val;
		return;
	case VNObjectPosition:
		state.mObjectPosition = val;
		return;
	case VNDiffuse:
		state.mDiffuse = val;
		return;
	case VNSpecular:
		state.mSpecular = val;
		return;
	case VNReflectivity:
		state.mReflectivity = val;
		return;
	case VNEmissive:
		state.mEmissive = val;
		return;
	case VNAbsorption:
		state.mAbsorption = val;
		return;
	case VNTrapPosition:
		state.mTrapPosition = val;
		return;
	}

	LOG(Error, "Missing variable in SetVectorValue");
	throw;
}

std::string DFuncFactory::GetParamType(EType type)
{
	switch(type)
	{
	case Vec:
		return "vec";
		break;
	case Float:
		return "float";
		break;
	case RawFloat:
		return "rawfloat";
		break;
	default:
		return "error";
	}
}

std::string DFuncFactory::WriteParams(std::vector<DFuncParam*> params)
{
	std::string ret;
	unsigned int i;
	std::stringstream temp;

	for (i=0; i<params.size(); i++)
	{
		temp << "<PARAM index=\"";
		temp << i;
		temp << "\" name=\"";
		temp << params[i]->mName;
		temp << "\" type=\"";
		temp << GetParamType(params[i]->mType);
		temp << "\"/>\n";
	}

	return temp.str();
}

void DFuncFactory::BuildSchema()
{
	mSchema.append("<DISTANCESCHEMA>\n");
	unsigned int i;
	for (i=0; i<mFuncList.size(); i++)
	{
		if (mFuncList[i]->GetType()==Float)
			mSchema.append("<FLOATFUNC name=\"" + mFuncList[i]->GetName() + "\">\n");
		else if (mFuncList[i]->GetType()==Vec)
			mSchema.append("<VECFUNC name=\"" + mFuncList[i]->GetName() + "\">\n");
		else if (mFuncList[i]->GetType()==Null)
			mSchema.append("<NULLFUNC name=\"" + mFuncList[i]->GetName() + "\">\n");

		mSchema.append(WriteParams(mFuncList[i]->mParam));

		if (mFuncList[i]->GetType()==Float)
			mSchema.append("</FLOATFUNC>");
		else if (mFuncList[i]->GetType()==Vec)
			mSchema.append("</VECFUNC>");
		else if (mFuncList[i]->GetType()==Null)
			mSchema.append("</NULLFUNC>");
	}
	for (i=0; i<mVariableList.size(); i++)
	{
		if (mVariableList[i]->GetType()==Float)
			mSchema.append("<FLOATVAR name=\"" + mVariableList[i]->GetDescription() + "\"/>\n");
		else if (mVariableList[i]->GetType()==Vec)
			mSchema.append("<VECVAR name=\"" + mVariableList[i]->GetDescription() + "\"/>\n");
	}
	mSchema.append("</DISTANCESCHEMA>");
}

DVariable* DFuncFactory::GetVariable(std::string& arg)
{
	for (unsigned int i=0; i<mVariableList.size(); i++)
	{
		if (arg.compare(mVariableList[i]->GetDescription())==0)
			return mVariableList[i];
	}

	return NULL;
}

bool DFuncFactory::IsFloatVar(std::string& token)
{
	DVariable* var = GetVariable(token);
	if (var && var->GetType()==Float)
		return true;
	else
		return false;
}

bool DFuncFactory::IsVectorVar(std::string& token)
{
	DVariable* var = GetVariable(token);
	if (var && var->GetType()==Vec)
		return true;
	else
		return false;
}

bool DFuncFactory::IsNullFunction(std::string& token)
{
	for (unsigned int i=0; i<mFuncList.size(); i++)
	{
		if (mFuncList[i]->GetType()==Null
			&& token.compare(mFuncList[i]->GetName())==0)
		{
			return true;
		}
	}

	return false;
}

DDistFunc* DFuncFactory::CreateFunction(std::string& arg)
{
	std::string token = ParseUtils::GetToken(arg);
	DDistFunc* ret = NULL;
	std::vector<DDistFunc*> params;
	
	if (!ParseUtils::IsOpenBracket(arg))
	{			
		// is it just a float?
		if (ParseUtils::IsFloat(token))
		{
			DDistFloat* floatFunc = new DDistFloat();
			floatFunc->SetFloat(ParseUtils::GetFloat(token));
			return floatFunc;
		}
		else if (IsFloatVar(token))
		{
			DDistFloatVar* floatVar = new DDistFloatVar();
			floatVar->SetVariable(GetVariable(token));
			return floatVar;
		}
		else if (IsVectorVar(token))
		{
			DDistVectorVar* vectorVar = new DDistVectorVar();
			vectorVar->SetVariable(GetVariable(token));
			return vectorVar;
		}
		else if (token.compare("{")==0)
		{
			DDistCodeblock* codeblock = new DDistCodeblock();
			arg = std::string("{").append(arg);
			codeblock->Parse(arg);
			return codeblock;
		}
	}
	else
	{
		ParseUtils::StripOpenBracket(arg);

		while(!ParseUtils::IsCloseBracket(arg))
		{
			params.push_back(CreateFunction(arg));
			if (ParseUtils::IsComma(arg))
				ParseUtils::StripComma(arg);
		}

		ParseUtils::StripCloseBracket(arg);
	}

	for(unsigned int i=0; i<mFuncList.size(); i++)
	{
		bool match = true;
		if (token.compare(mFuncList[i]->GetName())!=0)
		{
			match = false;
		}
		else
		{
			for (unsigned int arg=0; arg<params.size(); arg++)
			{
				if (mFuncList[i]->mParam[arg]->mType != params[arg]->GetType())
					match = false;					
			}
		}

		if (match)
		{
			ret = mFuncList[i]->Clone();
			ret->SetParams(params);
		}
	}

	if (!ret) {LOG(Error, "Missing Function Name");throw 1;}
	return ret;
}

std::vector<DNullFunc*> DFuncFactory::Create(std::string& arg)
{
	std::vector<DNullFunc*> ret;
	bool inParanthesis = false;

	while (arg.length()>0)
	{
		std::string token = ParseUtils::PeekToken(arg);
		if (token.compare("{")==0)
		{
			inParanthesis = true;
			token = ParseUtils::GetToken(arg);
		}
		if (token.compare("}")==0 && inParanthesis)
		{
			token = ParseUtils::GetToken(arg);
			return ret;
		}
		DDistFunc* func = CreateFunction(arg);
		if (func->GetType() == Null)
			ret.push_back((DNullFunc*)func);
	}

	return ret;
}