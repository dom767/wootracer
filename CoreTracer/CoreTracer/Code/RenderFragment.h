#pragma once

#include "Colour.h"

class DScene;
class DCamera;

class DRenderFragment
{
	DColour mPixels[256];
	const DScene* mScene;
	int mWidth;
	int mHeight;
	DCamera* mCamera;
public: int mLeft;
public: 	int mTop;
	float mVariance;
	int mSubFrame;
	bool mFastPreview;
	int mFastStage;

public:
	DRenderFragment(const DScene* scene, int width, int height, DCamera* camera, int left, int top, bool mFastPreview);
	void Render();
};