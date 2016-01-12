#include "stdafx.h"
#include "RenderFragment.h"
#include "Camera.h"

DRenderFragment::DRenderFragment(const DScene* scene, int width, int height, DCamera* camera, int left, int top, bool fastPreview)
	: mScene(scene), mWidth(width), mHeight(height), mCamera(camera), mLeft(left), mTop(top), mSubFrame(0), mFastPreview(fastPreview), mFastStage(fastPreview?0:5)
{
	int x, y;
	for (y=0; y<16; y++)
		for (x=0; x<16; x++)
			mPixels[x+16*y] = DColour(0.f, 0.f, 0.f);
	mVariance = 100000000000000.f; // bignum
}

void DRenderFragment::Render()
{
	mCamera->RenderFragment(*mScene, mSubFrame, mWidth, mHeight, mLeft, mTop, mPixels, mFastStage);
	if (mFastStage>=4)
		mSubFrame++;
	mCamera->CopyFragment(mWidth, mHeight, mLeft, mTop, mPixels, mFastStage<5?1:mSubFrame, mFastStage);
	if (mFastPreview && mFastStage<5)
		mFastStage++;
}
