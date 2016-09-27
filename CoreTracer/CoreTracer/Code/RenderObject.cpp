#include "stdafx.h"
#include "RenderObject.h"
#include "convert.h"
#include "tinyxml.h"
#include "Scene.h"

void DRenderObject::Read(TiXmlElement* element)
{
	Convert::StrToBool(element->Attribute("ignoreWhileLighting"), mIgnoreWhileLighting);
	Convert::StrToVec3(element->Attribute("position"), mCentre);
	Convert::StrToVec3(element->Attribute("scale"), mScale);
	Convert::StrToMat3(element->Attribute("rotation"), mRotation);

	mInvRotation = mRotation;
	mInvRotation.Invert();
	mScaleMat.MakeScaleMatrix(mScale);
	mInvScaleMat.MakeScaleMatrix(DVector3(1/mScale.mComponent[0],1/mScale.mComponent[1],1/mScale.mComponent[2]));

	DMatrix4 trans;
	trans.MakeIdentity();
	trans.mVal[3][0] = -mCentre[0];
	trans.mVal[3][1] = -mCentre[1];
	trans.mVal[3][2] = -mCentre[2];
	mCentreTrans = trans;
	mCentreTrans *= mInvRotation;
	mCentreTrans *= mInvScaleMat;
	mDirectionTrans = mInvRotation;
	mDirectionTrans *= mInvScaleMat;

	trans.mVal[3][0] = mCentre[0];
	trans.mVal[3][1] = mCentre[1];
	trans.mVal[3][2] = mCentre[2];
	mInvCentreTrans = mScaleMat;
	mInvCentreTrans *= mRotation;
	mInvCentreTrans *= trans;
	mInvDirectionTrans = mInvScaleMat;
	mInvDirectionTrans *= mRotation;

	TiXmlElement* materialXml = (TiXmlElement*) element->FirstChildElement("MATERIAL");
	mMaterial.Read(materialXml);

	InternalRead(element);
}

bool DRenderObject::Contains(const DVector3& position)
{
	DVector3 newPosition = mCentreTrans * position;
	return InternalContains(newPosition);
}

bool DRenderObject::Intersect(const DRayContext& rRayContext, DCollisionResponse& out_Response)
{
	DRayContext temp = rRayContext;
	temp.m_Ray.SetStart(mCentreTrans * temp.m_Ray.GetStart());

	temp.m_CameraPos = mCentreTrans * temp.m_CameraPos;
	 
	DVector3 newDirection = mDirectionTrans * temp.m_Ray.GetDirection();
	newDirection.Normalise();
	temp.m_Ray.SetDirection(newDirection);
	out_Response.mColour.mRed = out_Response.mColour.mGreen = out_Response.mColour.mBlue = 1.f;
	
	bool ret = InternalIntersect(temp, out_Response);

	if (ret)
	{
		out_Response.mHitPosition = mInvCentreTrans * out_Response.mObjectPosition;
		out_Response.mNormal = mInvDirectionTrans * out_Response.mNormal;
		out_Response.mNormal.Normalise();
		out_Response.mDistance = (out_Response.mHitPosition - rRayContext.m_Ray.GetStart()).Magnitude();

		if (rRayContext.m_RequestFlags&RequestZeroLighting && GetIgnoreWhileLighting()) // pathtracing ignores lightsources as light contributions (lighting is directly sampled)
		{
			out_Response.mColour = DColour(0,0,0);
		}
		else if (rRayContext.m_RequestFlags&RequestColour) // colour requsted (expensive!)
		{
			mMaterial.CalculateColour(out_Response.mColour, out_Response.mHitPosition, out_Response.mObjectPosition, GetObjectId(), out_Response.mNormal, rRayContext, mIgnoreWhileLighting);
		}
	}

	return ret;
}

void DRenderObject::CalculateColour(const DRayContext& rRayContext, DCollisionResponse& out_Response)
{
	if (rRayContext.m_RequestFlags&RequestZeroLighting && GetIgnoreWhileLighting()) // pathtracing ignores lightsources as light contributions (lighting is directly sampled)
	{
		out_Response.mColour = DColour(0,0,0);
	}
	else if (rRayContext.m_RequestFlags&RequestColour) // colour requsted (expensive!)
	{
		mMaterial.CalculateColour(out_Response.mColour, out_Response.mHitPosition, out_Response.mObjectPosition, GetObjectId(), out_Response.mNormal, rRayContext, mIgnoreWhileLighting);
	}
//	mMaterial.CalculateColour(out_Response.mColour, out_Response.mHitPosition, GetObjectId(), out_Response.mNormal, rRayContext, mIgnoreWhileLighting);
}

DRenderObject* DRenderObject::Clone()
{
	DRenderObject* ret = InternalClone();
/*	ret->mAABoundingBox = mAABoundingBox;
	ret->mCentre = mCentre;
	ret->mIgnoreWhileLighting = mIgnoreWhileLighting;
	ret->mMaterial = mMaterial;
	ret->mObjectId = mObjectId;
	ret->mObjectName = mObjectName;
	ret->mScale = mScale;
	ret->mTransform = mTransform;
*/	return ret;
}