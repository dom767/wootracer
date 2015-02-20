#include "stdafx.h"
#include "Viewport.h"
#include "tinyxml.h"
#include "convert.h"
#include "Scene.h"
#include "RenderSphere.h"
#include "Camera.h"

void DViewport::Read(TiXmlElement* element)
{
	Convert::StrToInt(element->Attribute("width"), mWidth);
	Convert::StrToInt(element->Attribute("height"), mHeight);
}

void DViewport::Render(bool singleFrame)
{
	mCamera.Render(mScene, mWidth, mHeight, (DColour*)mBuffer, singleFrame);
}

void DViewport::SetBuffer(float* buffer)
{
	if (mBuffer)
		delete [] mBuffer;

	mBuffer = buffer;
}

void DViewport::CopyBuffer(DColour* buffer)
{
	mCamera.CopyBuffer(buffer);
}
/*
void DViewport::RenderPatch(int x, int y)
{
	mScene.PrepareRender();
	mCamera.RenderPatch(mScene, mWidth, mHeight, (DColour*)mBuffer, x, y);
}
*/
float DViewport::GetDepth(int width, int height)
{
	if (width>mWidth || height>mHeight)
		return -2;

	mScene.PrepareRender();
	return mCamera.GetDepth(mScene, mWidth, mHeight, width, height);
}