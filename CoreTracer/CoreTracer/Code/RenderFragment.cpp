#include "stdafx.h"
#include "RenderFragment.h"
#include "Camera.h"

DRenderFragment::DRenderFragment(const DScene* scene, int width, int height, DCamera* camera, int left, int top)
	: mScene(scene), mWidth(width), mHeight(height), mCamera(camera), mLeft(left), mTop(top), mSamples(0)
{
	int x, y;
	for (y=0; y<16; y++)
		for (x=0; x<16; x++)
			mPixels[x+16*y] = DColour(0.f, 0.f, 0.f);
	mVariance = 100000000000000.f; // bignum
}

void DRenderFragment::Render()
{
	mCamera->RenderFragment(*mScene, mWidth, mHeight, mLeft, mTop, mPixels);
	mSamples++;
	mCamera->CopyFragment(mWidth, mHeight, mLeft, mTop, mPixels, mSamples);
}
