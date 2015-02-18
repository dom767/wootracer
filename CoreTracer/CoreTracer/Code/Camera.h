#pragma once

#include "Vector3.h"
#include "Matrix4.h"
#include "SceneObject.h"
#include "Ray.h"
#include "WorkQueue.h"
#include <list>

class DScene;
class DColour;
class TiXmlElement;
class DCamera;
class DRenderFragment;
class DWorkQueue;

class DProgressMonitor
{
public:
	virtual void SetProgress(float progress)=0;
};

class DCamera
{
public:
	DCamera();

	void Read(TiXmlElement* element);

	DVector3 mFrom;
	DVector3 mTarget;
	DVector3 mUp;
	DMatrix4 mViewMatrix;
	float mFOV;
	bool mDOFEnabled;
	bool mAAEnabled;
	float mFocusDepth;
	float mApertureSize;
	float mSpherical;
	int mMinSamples;
	int mMaxSamples;
	DProgressMonitor* mProgressMonitor;
	DColour* mBuffer;
	DColour* mScratchBuffer;
	CRITICAL_SECTION mCS;
	DWorkQueue mWorkQueue;
	int mWidth;
	int mHeight;
	mutable __int64 mPixelCount;
	mutable __int64 mPatchSampleCount;
	HANDLE mRenderHandle;

	/// Copy current buffer into provided external buffer
	void CopyBuffer(DColour* buffer);
	void CopyFragment(int width, int height, int left, int top, DColour* pixels, int samples);
	void LockingCopy(DColour* source, DColour* dest, int offset, int size);
	void StopRender();

	void Render(const DScene& scene, const int width, const int height, DColour *renderBuffer, bool singleFrame);
	float GetDepth(const DScene& scene, const int width, const int height, const int x, const int y);
	void RenderPatch(const DScene& scene, const int width, const int height, DColour *renderBuffer, int x, int y);

	void RenderRow(const DScene& scene, const int width, const int height, const int row, DColour *renderBuffer);
	void RenderFragment(const DScene& scene, const int width, const int height, const int left, const int top, DColour *renderBuffer);
	void CalculateCameraMatrix();
	void SetProgressMonitor(DProgressMonitor* progressMonitor) {mProgressMonitor=progressMonitor;}
	bool DCamera::GetRay(DRay& out_ray, const DVector3 from, const int width, const int height, const float x, const float y, const DVector3& jitter);

private:

};