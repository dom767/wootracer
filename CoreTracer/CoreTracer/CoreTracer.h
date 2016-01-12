#ifndef CORETRACER_H
#define CORETRACER_H

#include "resource.h"
#include "Code\Viewport.h"
#include "Code\Camera.h"
#include "Code\Scene.h"
#include "PostProcess.h"

//extern "C" __declspec(dllexport) bool Render(float* buffer, const char* description);
//extern "C" __declspec(dllexport) bool GetDepth(float& depth, const char* description, int x, int y);
extern "C" __declspec(dllexport) void InitialiseRender(const char* description);
extern "C" __declspec(dllexport) void SetCamera(const char* description);
extern "C" __declspec(dllexport) void SetViewport(const char* description);
extern "C" __declspec(dllexport) void StartRender();
extern "C" __declspec(dllexport) void StopRender();
extern "C" __declspec(dllexport) void CopyBuffer(float* buffer);
extern "C" __declspec(dllexport) void SyncRender(float* buffer);
extern "C" __declspec(dllexport) void GetDepth(float& depth, int x, int y);
extern "C" __declspec(dllexport) int GetDistanceSchemaLength();
extern "C" __declspec(dllexport) void GetDistanceSchema(char* schema);
extern "C" __declspec(dllexport) void PostProcess(float* targetBuffer, float* sourceBuffer, double maxValue, int iterations, float* kernel, float boostPower, float targetweighting, float sourceweighting, int width, int height);
extern "C" __declspec(dllexport) void GaussianBlur(float* targetBuffer, float* sourceBuffer, double maxValue, int size, float boostPower, float targetWeighting, float sourceWeighting, int width, int height);

//bool RenderPatch(float* buffer, const char* description, int x, int y);

class DCoreTracer
{
public:
	DCoreTracer() : mViewport(mCamera, mScene), mBuffer(NULL), mStarted(0) {}
	~DCoreTracer() {delete[] mBuffer;}

	void BuildFromXml(const char* const description);
	void BuildCamera(const char* const description);
	void BuildViewport(const char* const description);
	void SetBuffer(float* buffer) {mViewport.SetBuffer(buffer);}
	//void Render();
//	void RenderPatch(int x, int y);
	float GetDepth(int width, int height, bool startedRender);
	int GetWidth() {return mViewport.GetWidth();}
	int GetHeight() {return mViewport.GetHeight();}
	void StartRender(bool singleFrame);
	void StopRender();
	void CopyBuffer(float* buffer);
	void Render(bool singleFrame) {mViewport.Render(singleFrame);}
	void PostProcess(float* targetBuffer, float* sourceBuffer, double maxValue, int iterations, float* kernel, float boostPower, float targetWeighting, float sourceWeighting, int width, int height)
	{
		mPostProcess.PostProcess(targetBuffer, sourceBuffer, maxValue, iterations, kernel, boostPower, targetWeighting, sourceWeighting, width, height);
	}
	void GaussianBlur(float* targetBuffer, float* sourceBuffer, double maxValue, int size, float boostPower, float targetWeighting, float sourceWeighting, int width, int height)
	{
		mPostProcess.GaussianBlur(targetBuffer, sourceBuffer, maxValue, size, boostPower, targetWeighting, sourceWeighting, width, height);
	}
	__int64 GetRayCount();
	__int64 GetKDCount();
	__int64 GetPixelCount();
	__int64 GetPatchSamples();
	__int64 GetIntersectionCount();

private:
	float* mBuffer;
	DPostProcess mPostProcess;
	DViewport mViewport;
	DCamera mCamera;
	DScene mScene;
	volatile unsigned int mStarted;

};

#endif // CORETRACER_H
