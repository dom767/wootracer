#include "stdafx.h"
#include "Camera.h"
#include "Colour.h"
#include "Ray.h"
#include "Scene.h"
#include "SceneIO.h"
#include "tinyxml.h"
#include "convert.h"
#include "Vector2.h"
#include <tchar.h>
#include <list>
#include <windows.h>
#include <iostream>
#include <fstream>
#include "log.h"
#include "RenderFragment.h"


void DCamera::Read(TiXmlElement* element)
{
	Convert::StrToVec3(element->Attribute("from"), mFrom);
	Convert::StrToVec3(element->Attribute("target"), mTarget);
	Convert::StrToVec3(element->Attribute("up"), mUp);
	Convert::StrToFloat(element->Attribute("fov"), mFOV);
	Convert::StrToBool(element->Attribute("dofEnabled"), mDOFEnabled);
	Convert::StrToBool(element->Attribute("aaEnabled"), mAAEnabled);
	Convert::StrToFloat(element->Attribute("focusDepth"), mFocusDepth);
	Convert::StrToFloat(element->Attribute("apertureSize"), mApertureSize);
	Convert::StrToInt(element->Attribute("minSamples"), mMinSamples);
	Convert::StrToInt(element->Attribute("maxSamples"), mMaxSamples);
	Convert::StrToFloat(element->Attribute("spherical"), mSpherical);
}

struct ThreadArgs
{
	ThreadArgs(DWorkQueue& workQueue, bool singleFrame, int thread)
		: mWorkQueue(workQueue), mSingleFrame(singleFrame), mThread(thread)
	{
		mRenderFragment = workQueue.GetHead();
	}

	DRenderFragment* mRenderFragment;
	DWorkQueue& mWorkQueue;
	bool mSingleFrame;
	int mThread;
	int mSubFrame;

	void Render()
	{
		mRenderFragment->Render();
		
		static WCHAR dest[256];
		swprintf(dest, 256, L"Thread %i rendered square at %i, %i\n", mThread, mRenderFragment->mLeft, mRenderFragment->mTop);
		OutputDebugString(&dest[0]);

		static char bdest[256];
		sprintf_s(bdest, "Thread %i rendered square at %i, %i\n", mThread, mRenderFragment->mLeft, mRenderFragment->mTop);
		Log().Print(&bdest[0]);
	}

	bool GetNext()
	{
		if (!mSingleFrame)
			mWorkQueue.Update(mRenderFragment);
		else
			delete mRenderFragment;

		mRenderFragment = mWorkQueue.GetHead();

		if (mRenderFragment==0)
			return false;
		else
			return true;
	}
};

DWORD WINAPI RenderRow_Thread(LPVOID arg)
{
	ThreadArgs* threadArgs = (ThreadArgs*)(arg);

	if (threadArgs->mRenderFragment == NULL)
		return 1;

	do
	{
		threadArgs->Render();
	}
	while(threadArgs->GetNext());

	return 1;
}

DCamera::DCamera() : mProgressMonitor(0),
	mMinSamples(32),
	mMaxSamples(128),
	mDOFEnabled(false),
	mAAEnabled(false),
	mFocusDepth(1.0),
	mApertureSize(0.0),
	mPixelCount(0),
	mPatchSampleCount(0),
	mSpherical(0)
{
	InitializeCriticalSection(&mCS);
	mRenderHandle = CreateEvent(NULL, TRUE, TRUE, _T("RenderHandle"));
}

void DCamera::Render(const DScene& scene, const int width, const int height, DColour *renderBuffer, bool singleFrame)
{
	Log().Print("Camera.Render");
	DWORD result = WaitForSingleObject(mRenderHandle,1);
	if (result==WAIT_TIMEOUT)
		return;

	ResetEvent(mRenderHandle);
//	WaitForSingleObject(mRenderHandle, INFINITE);
//	mScratchBuffer = (DColour*)(new float[width*height*3]);
	mBuffer = (DColour*)renderBuffer;
	mWidth = width;
	mHeight = height;

	Log().Print("Camera.Render InitialiseRenderQueue");
	for (int y=0; y<=height/16; y++)
	{
		for (int x=0; x<=width/16; x++)
		{
			mWorkQueue.Add(new DRenderFragment(&scene, width, height, this, x*16, y*16));
		}
	}

	Log().Print("Camera.Render InitialiseThreads");
#ifdef _DEBUG
	const int NumThreads = 1;
#else
	const int NumThreads = 4;
#endif
//	DScene sceneClones[NumThreads];
	CalculateCameraMatrix();

	HANDLE threadHandle[NumThreads];
	DWORD threadId[NumThreads];
	int row = 0;
	std::vector<ThreadArgs*> args;
	for (int i=0; i<NumThreads; i++)
	{
		static char bdest[256];
		sprintf_s(bdest, "Camera.Render Initialise Thread %i", i);
		Log().Print(&bdest[0]);

//		sceneClones[i] = scene;
		scene.SetRandomSeed((rand()<<16)|rand());
//		sceneClones[i].SetRandomSeed((rand()<<16)|rand());
		args.push_back(new ThreadArgs(mWorkQueue, singleFrame, i));
		threadHandle[i] = CreateThread(NULL, 0, RenderRow_Thread, args[i], 0, &threadId[i]);
		if (threadHandle[i] == 0)
		{
			Log().Print("Camera.Render CreateThread failed");
			DWORD lastError = GetLastError();
			static char bdest[256];
			sprintf_s(bdest, "WaitFailed with error %i", lastError);
			Log().Print(&bdest[0]);
		}
	}

	int finishedThread = WaitForMultipleObjects(NumThreads, threadHandle, true, INFINITE);
	if (finishedThread==-1)
	{
		Log().Print("Camera.Render WaitFailed");
		DWORD lastError = GetLastError();
		static char bdest[256];
		sprintf_s(bdest, "WaitFailed with erorr %i", lastError);
		Log().Print(&bdest[0]);
	}
	Log().Print("Camera.Render ThreadsComplete");

	SetEvent(mRenderHandle);
	mRenderHandle = NULL;

	for (int i=0; i<NumThreads; i++)
	{
		CloseHandle(threadHandle[i]);
		delete args[i];
	}
	Log().Print("Camera.Render ClearedHandles");
}

void DCamera::StopRender()
{
	mWorkQueue.SetStop();
	WaitForSingleObject(mRenderHandle, INFINITE);
}

void DCamera::CopyBuffer(DColour* buffer)
{
	LockingCopy(buffer, mBuffer, 0, mWidth*mHeight);
}

void DCamera::CopyFragment(int width, int height, int left, int top, DColour* pixels, int samples)
{
	EnterCriticalSection(&mCS);
	int startOffset = left + top*width;

	int maxx = width-left;
	if (maxx>16) maxx=16;
	int maxy = height-top;
	if (maxy>16) maxy=16;

	for (int y=0; y<maxy; y++)
	{
		for (int x=0; x<maxx; x++)
		{
			if (left+x<width && top+y<height)
				mBuffer[startOffset + y*width + x] = pixels[y*16 + x] / (float)samples;
		}
	}
	LeaveCriticalSection(&mCS);
}

void DCamera::LockingCopy(DColour* dest, DColour* source, int offset, int size)
{
	EnterCriticalSection(&mCS);
	int byteoffset = offset*sizeof(DColour);
	int bytetransfer = size*sizeof(DColour);
	memcpy(((char*)dest)+byteoffset, ((char*)source)+byteoffset, bytetransfer);
	LeaveCriticalSection(&mCS);
}

bool DCamera::GetRay(DRay& out_ray, const DVector3 from, const int width, const int height, const float x, const float y, const DVector3& jitter)
{
	// calculate ray origin
	DVector3 jitterWorld = mViewMatrix*jitter;
	out_ray.SetStart(from+jitterWorld);

	// calculate vector away from camera
	float aspectRatio = float(width)/float(height);
	DVector3 direction = DVector3((float(x)/float(width)-0.5f)*mFOV,
		((-0.5f/aspectRatio)+float(y)/(float(width)))*mFOV, 45 * 0.5f);
	direction.Normalise();

	// do spherical distortion (spherical == 1 == fisheye)
	if (abs(mSpherical)>0.0001f)
	{
		float sx = float(x)/float(width) - 0.5f;
		float sy = float(y)/float(width) - 0.5f/aspectRatio;

		float mag = sqrtf(sx*sx + sy*sy);

		if (mag*(mFOV/90)>0.5)
			return false;

		// normalise
		sx/=mag;
		sy/=mag;

		// adjust by FOV
		mag *= mFOV*3.14159265f/90;

		DVector3 sDirection = DVector3(sx * sin(mag), sy * sin(mag), cos(mag));
		direction += (sDirection - direction) * mSpherical;
	}

	// Find focus point
	float scale = mFocusDepth / direction.mComponent[2];
	DVector3 focusedPoint = direction * scale;
	
	// Calculate new direction from origin to focus point
	DVector3 jitterDirection = focusedPoint - jitter;
	jitterDirection.Normalise();

	// rotate direction into view space
	jitterDirection = mViewMatrix*jitterDirection;
	out_ray.SetDirection(jitterDirection);

	return true;
}

float DCamera::GetDepth(const DScene& scene, const int width, const int height, const int x, const int y)
{
	CalculateCameraMatrix();

	scene.SetRandomSeed((rand()<<16)|rand());

	DRay ray;
	bool success = GetRay(ray, mFrom, width, height, (float)x, (float)y, DVector3(0,0,0));

	bool hit = false;
	DCollisionResponse response;

	if (success)
	{
		hit = scene.Intersect(DRayContext(&scene, ray, RequestColour, scene.GetMaximumRecursion(), DRayContext::AirRefractionIndex, width*y+x, 0), response);
	}

	if (hit)
		return response.mDistance;
	else
		return -1;
}

struct Patch
{
	Patch(float bottom, float top, float left, float right, const DScene& scene, bool dofEnabled, bool aaEnabled) :
		mBottom(bottom),
		mTop(top),
		mLeft(left),
		mRight(right),
		mScene(scene),
		mCalculated(false),
		mSignificance(1.0),
		mDOFEnabled(dofEnabled),
		mAAEnabled(aaEnabled) {}

	~Patch()
	{
	}

	void Sample(DCamera& camera, int subframe, float apertureSize, DVector3& from, int width, int height, int x, int y)
	{
		DVector3 dofJitter;
		float aax=0.0f, aay=0.0f;
		int pixelIndex = x+y*width;

		if (mDOFEnabled)
		{
			DVector2 randView = mScene.GetRandom2D(0, pixelIndex, subframe);
			float randx = randView.x;
			float randy = randView.y;

			dofJitter.mComponent[0] = apertureSize * (mLeft + (mRight-mLeft) * randx);
			dofJitter.mComponent[1] = apertureSize * (mBottom + (mTop-mBottom) * randy);
		}

		if (mAAEnabled)
		{
			if (mDOFEnabled)
			{
				DVector2 randDOF = mScene.GetRandom2D(2, pixelIndex, subframe);
				aax = randDOF.x;
				aay = randDOF.y;
			}
			else
			{
				DVector2 randAA = mScene.GetRandom2D(0, pixelIndex, subframe);
				aax = mLeft + (mRight-mLeft) * (randAA.x);
				aay = mBottom + (mTop-mBottom) * (randAA.y);
			}
			mSamplePosX = aax;
			mSamplePosY = aay;
		}

		DRay ray;
		bool success = camera.GetRay(ray,
			from,
			width,
			height,
			(float)x+(aax-0.5f),
			(float)y+(aay-0.5f),
			dofJitter);

		bool hit = false;

		if (success)
		{
			bool debugInfo = false;
			if (x==318 && y == 58)
			{
				debugInfo = true;
			}
			DCollisionResponse response;
			hit = mScene.Intersect(DRayContext(&mScene, ray, RequestColour, mScene.GetMaximumRecursion(), DRayContext::AirRefractionIndex, pixelIndex, subframe), response, debugInfo);
			mColour = response.mColour;
		}
		else
		{
			mColour = DColour(0,0,0);
		}

		mCalculated = true;
	}

	void SampleChildren(DCamera& camera, int subframe, float apertureSize, DVector3& from, int width, int height, int x, int y)
	{
		float halfHeight = 0.5f * (mTop + mBottom) ;
		float halfWidth = 0.5f * (mRight + mLeft);
		int idx=0;
		
		if (mSamplePosX>(mLeft+mRight)*0.5)
			idx += 1;
		if (mSamplePosY>(mTop+mBottom)*0.5)
			idx += 2;

		mChild[0].reset(new Patch(mBottom, halfHeight, mLeft, halfWidth, mScene, mDOFEnabled, mAAEnabled));
		mChild[1].reset(new Patch(mBottom, halfHeight, halfWidth, mRight, mScene, mDOFEnabled, mAAEnabled));
		mChild[2].reset(new Patch(halfHeight, mTop, mLeft, halfWidth, mScene, mDOFEnabled, mAAEnabled));
		mChild[3].reset(new Patch(halfHeight, mTop, halfWidth, mRight, mScene, mDOFEnabled, mAAEnabled));

		for (int i=0; i<4; i++)
		{
			if (i==idx)
				mChild[idx]->SetColour(mColour);
			else
				mChild[i]->Sample(camera, subframe, apertureSize, from, width, height, x, y);
		}

		mColour = DColour(0,0,0);
		for (int i=0; i<4; i++)
		{
			mColour += mChild[i]->mColour;
		}
		mColour *= 0.25;

		for (int i=0; i<4; i++)
		{
			mChild[i]->CalculateSignificance(mColour);
		}
	}

	void CalculateSignificance(DColour colour)
	{
		if (mChild[0].get() == NULL)
		{
			float rdiff = mColour.mRed - colour.mRed;
			float gdiff = mColour.mGreen - colour.mGreen;
			float bdiff = mColour.mBlue - colour.mBlue;
			mSignificance = (rdiff * rdiff + gdiff * gdiff + bdiff * bdiff)
				* (mRight-mLeft) * (mTop-mBottom);
		}
	}

	void SetColour(DColour setColour)
	{
		mColour = setColour;
		mCalculated = true;
	}

	float GetDiff(DColour colour1, DColour colour2)
	{
		float rdiff = colour1.mRed - colour2.mRed;
		float gdiff = colour1.mGreen - colour2.mGreen;
		float bdiff = colour1.mBlue - colour2.mBlue;
		return rdiff*rdiff + gdiff*gdiff + bdiff*bdiff;
	}

	float GetVariance(DColour& colour)
	{
		if (!mChild[0].get())
			return GetDiff(mColour, colour);
		else
		{
			return (mChild[0]->GetVariance(colour) + 
				mChild[1]->GetVariance(colour) + 
				mChild[2]->GetVariance(colour) + 
				mChild[3]->GetVariance(colour)) * 0.25f; 
		}
	}

	DColour GetColour()
	{
		if (!mChild[0].get())
			return mColour;
		else
		{
			return (mChild[0]->GetColour() + 
				mChild[1]->GetColour() + 
				mChild[2]->GetColour() + 
				mChild[3]->GetColour()) * 0.25f; 
		}
	}

	float mBottom;
	float mTop;
	float mLeft;
	float mRight;
	float mSamplePosX;
	float mSamplePosY;
	float mSignificance;
	DColour mColour;
	std::auto_ptr<Patch> mChild[4];
	Patch* mParent;
	const DScene& mScene;
	bool mCalculated;
	bool mAAEnabled;
	bool mDOFEnabled;
};

class PatchRenderer
{
public:
	PatchRenderer(Patch& parent, bool dofEnabled, bool aaEnabled, int minSamples, int maxSamples) : mSamples(0),
		mParent(parent),
		mDOFEnabled(dofEnabled),
		mAAEnabled(aaEnabled),
		mMinSamples(minSamples),
		mMaxSamples(maxSamples)
	{
		mPatch.push_back(&parent);
	}

	static bool SortFn(Patch* lhs, Patch* rhs)
	{
		if (lhs->mSignificance<rhs->mSignificance)
			return false;
		else
			return true;
	}

	void Render(DCamera& camera, int subFrame, float apertureSize, DVector3& from, int width, int height, int x, int y)
	{
		(*mPatch.begin())->Sample(camera, subFrame, apertureSize, from, width, height, x, y);
		mSamples = 1;

		while (((*mPatch.begin())->mSignificance>0.0000000001 || mSamples<mMinSamples) && mSamples<mMaxSamples)
		{
			mSamples += 3;
			(*mPatch.begin())->SampleChildren(camera, subFrame, apertureSize, from, width, height, x, y);
			mPatch.push_back((*mPatch.begin())->mChild[0].get());
			mPatch.push_back((*mPatch.begin())->mChild[1].get());
			mPatch.push_back((*mPatch.begin())->mChild[2].get());
			mPatch.push_back((*mPatch.begin())->mChild[3].get());
			mPatch.remove(*(mPatch.begin()));

//			mPatch.sort(SortFn);
		}
	}

	std::list<Patch*> mPatch;
	Patch& mParent;
	int mSamples;
	int mMinSamples;
	int mMaxSamples;
	bool mDOFEnabled;
	bool mAAEnabled;
};
/*
void DCamera::RenderRow(const DScene& scene, const int width, const int height, const int row, DColour *renderBuffer)
{
	int x,nrow=row;

	CalculateCameraMatrix();

	for (x=0; x<width; x++)
	{
		renderBuffer[nrow*width+x] = DColour(0,0,0);

		Patch parent(0.0, 1.0, 0.0, 1.0, scene, mDOFEnabled, mAAEnabled);

		if (mDOFEnabled || mAAEnabled)
		{
			PatchRenderer patchRenderer(parent, mDOFEnabled, mAAEnabled, mMinSamples, mMaxSamples);
			patchRenderer.Render(*this, mApertureSize, mFrom, width, height, x, nrow);
		}
		else
		{
			parent.Sample(*this, 0, mFrom, width, height, x, nrow);
		}

//		DColour avg = parent.GetColour();
//		float var = 10 * parent.GetVariance(avg);
//		renderBuffer[nrow*width+x] = DColour(var, var, var);

		renderBuffer[nrow*width+x] = parent.GetColour();
	}
}
*/
void DCamera::RenderFragment(const DScene& scene, const int subframe, const int width, const int height, const int left, const int top, DColour *renderBuffer)
{
	int x, y;

	CalculateCameraMatrix();

	int maxx = width-left;
	if (maxx>16) maxx=16;
	int maxy = height-top;
	if (maxy>16) maxy=16;

	for (y=0; y<maxy; y++)
	{
		for (x=0; x<maxx; x++)
		{
			Patch parent(0.0, 1.0, 0.0, 1.0, scene, mDOFEnabled, mAAEnabled);
			int samples = 1;

			if (mDOFEnabled || mAAEnabled)
			{
				PatchRenderer patchRenderer(parent, mDOFEnabled, mAAEnabled, mMinSamples, mMaxSamples);
				patchRenderer.Render(*this, subframe, mApertureSize, mFrom, width, height, left+x, top+y);
				samples = patchRenderer.mSamples;
			}
			else
			{
				parent.Sample(*this, subframe, 0, mFrom, width, height, left+x, top+y);
			}

	//		DColour avg = parent.GetColour();
	//		float var = 10 * parent.GetVariance(avg);
	//		renderBuffer[nrow*width+x] = DColour(var, var, var);

			mPatchSampleCount += samples;
			renderBuffer[y*16+x] += parent.GetColour();
		}
	}

	mPixelCount+= maxx * maxy;
}

void DCamera::CalculateCameraMatrix()
{
	DVector3 direction = mTarget - mFrom;
	direction.Normalise();
	DVector3 up = mUp;
	DVector3 right = up.Cross(direction);
	right.Normalise();
	up = right.Cross(direction);
	mViewMatrix.mVal[0][0] = right[0];
	mViewMatrix.mVal[0][1] = right[1];
	mViewMatrix.mVal[0][2] = right[2];
	mViewMatrix.mVal[0][3] = 0.0f;
	mViewMatrix.mVal[1][0] = up[0];
	mViewMatrix.mVal[1][1] = up[1];
	mViewMatrix.mVal[1][2] = up[2];
	mViewMatrix.mVal[1][3] = 0.0f;
	mViewMatrix.mVal[2][0] = direction[0];
	mViewMatrix.mVal[2][1] = direction[1];
	mViewMatrix.mVal[2][2] = direction[2];
	mViewMatrix.mVal[2][3] = 0.0f;
	mViewMatrix.mVal[3][0] = 0.0f;
	mViewMatrix.mVal[3][1] = 0.0f;
	mViewMatrix.mVal[3][2] = 0.0f;
	mViewMatrix.mVal[3][3] = 1.0f;
}

void TransferPatch(Patch& patch, DColour* renderBuffer, int width, int height)
{
	if (!patch.mChild[0].get())
	{
		for (int yi=int(float(height)*patch.mBottom); yi<int(float(height)*patch.mTop); yi++)
		{
			for (int xi=int(float(width)*patch.mLeft); xi<int(float(width)*patch.mRight); xi++)
			{
				renderBuffer[yi*width+xi] = patch.GetColour();
			}
		}
	}
	else
	{
		TransferPatch(*(patch.mChild[0]), renderBuffer, width, height);
		TransferPatch(*(patch.mChild[1]), renderBuffer, width, height);
		TransferPatch(*(patch.mChild[2]), renderBuffer, width, height);
		TransferPatch(*(patch.mChild[3]), renderBuffer, width, height);
	}
}
/*
void DCamera::RenderPatch(const DScene& scene, const int width, const int height, DColour *renderBuffer, int x, int y)
{
	CalculateCameraMatrix();

    Patch parent(0.0, 1.0, 0.0, 1.0, scene, true, true);

	PatchRenderer patchRenderer(parent, mDOFEnabled, mAAEnabled, mMinSamples, mMaxSamples);
	patchRenderer.Render(*this, mApertureSize, mFrom, width, height, x, y);

	TransferPatch(parent, renderBuffer, width, height);
}
*/