// CoreTracer.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CoreTracer.h"
#include "tinyxml.h"
#include "Code\Scene.h"
#include <shlobj.h>
#include <iostream>
#include <fstream>
#include "Code\log.h"
#include "Code\FuncFactory.h"

// Other include files, declarations, and non-Xerces-C++ initializations.

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
int gWidth;
int gHeight;
float* gBuffer;
float* gDestBuffer;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
bool gDone = false;


/*int _tmain(int argc, _TCHAR* argv[])
{
	float buffer[271*271*3];
	Render(buffer, 271, 271, "\r\n<VIEWPORT width=210 height=121/><CAMERA from=\"0, 20, -20\" target=\"0, 0, 0\" up=\"0, 1, 0\" fov=\"40\" /><SCENE>\r\n  <LIGHT position=\"0, 20, 0\" colour=\"100.1, 100.1, 100.1\" />\r\n  <OBJECT type=\"0\" size=\"5\" centre=\"0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"1\" size=\"25\" normal=\"0, 1, 0\" centre=\"-29.75, 0, 1.5\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n</SCENE>");
/*	Render(buffer, 2, 2, "\
<VIEWPORT width=2 height=2/>\
<CAMERA from=\"0,0,-10\" target=\"0,0,0\" up=\"0,1,0\" fov=40/>\
<SCENE>\
	<LIGHT position=\"10, 10, -10\" colour=\"1.0, 1.0, 0.0\"/>\
	<LIGHT position=\"-10, 10, -10\" colour=\"0.0, 0.0, 1.0\"/>\
	<OBJECT type=0 size=\"2\" centre=\"0, 0, 0\" ignoreWhileLighting=\"false\">\
		<MATERIAL specularPower=\"5\" opacity=\"1.0\" density=\"1\">\
			<DIFFUSECOLOUR type=0 colour=\"1.0, 1.0, 1.0\"/>\
			<SPECULARCOLOUR type=0 colour=\"1.0, 1.0, 1.0\"/>\
			<EMISSIVECOLOUR type=0 colour=\"0.0, 0.0, 0.0\"/>\
			<REFLECTIVITYCOLOUR type=0 colour=\"1.0, 1.0, 1.0\"/>\
			<ABSORPTIONCOLOUR type=0 colour=\"1.0, 1.0, 1.0\"/>\
		</MATERIAL>\
	</OBJECT>\
</SCENE>");*//*
	return 0;
}
*/

void DCoreTracer::BuildCamera(const char* const description)
{
	LOG(Error, "Coretracer.BuildCamera");
	TiXmlDocument xml;
	xml.Parse(description);
	TiXmlElement* first = (TiXmlElement*)xml.FirstChildElement("CAMERA");
	mCamera.Read(first);
}

void DCoreTracer::BuildViewport(const char* const description)
{
	LOG(Error, "Coretracer.BuildViewport");
	TiXmlDocument xml;
	xml.Parse(description);
	TiXmlElement* first = (TiXmlElement*)xml.FirstChildElement("VIEWPORT");
	mViewport.Read(first);

	float* buffer = new float[GetWidth() * 3 * GetHeight()];
	SetBuffer(buffer);
}

void DCoreTracer::BuildFromXml(const char* const description)
{
	LOG(Error, "Coretracer.BuildFromXml");
	TiXmlDocument xml;
	xml.Parse(description);
	TiXmlElement* first = (TiXmlElement*) xml.FirstChildElement("VIEWPORT");
	mViewport.Read(first);
	first = (TiXmlElement*) xml.FirstChildElement("CAMERA");
	mCamera.Read(first);
	first = (TiXmlElement*) xml.FirstChildElement("SCENE");
	mScene.Read(first);

	mScene.PrepareRender();
}
/*
void DCoreTracer::RenderPatch(int x, int y)
{
	LOG(Error, "Coretracer.RenderPatch");
	mViewport.RenderPatch(x, y);
}
*/
float DCoreTracer::GetDepth(int width, int height, bool startedRender)
{
	LOG(Error, "Coretracer.GetDepth");
	return mViewport.GetDepth(width, height, startedRender);
}

void DCoreTracer::CopyBuffer(float* buffer)
{
	LOG(Error, "Coretracer.CopyBuffer");
	mViewport.CopyBuffer((DColour*)buffer);
}

DWORD WINAPI CTRender(LPVOID arg)
{
	((DCoreTracer*)arg)->Render(false);
	return 1;
}		

HANDLE threadHandle;

DCoreTracer* g_CoreTracer=NULL;

void DCoreTracer::StartRender(bool singleFrame)
{
	LOG(Error, "Coretracer.StartRender");
	DWORD threadId;
	
	if (singleFrame)
	{
		static int callCount=0;
		std::wstringstream ss;
		ss << L"Starting single frame render " << callCount << L"\n";
		OutputDebugString(ss.str().c_str());
		g_CoreTracer->Render(true);
		OutputDebugString(L"Finished single frame render\n");
		callCount++;
		mStarted = 0;
	}
	else
	{
		OutputDebugString(L"Starting continuous render\n");
		threadHandle = CreateThread(NULL, 0, CTRender, g_CoreTracer, 0, &threadId);	
	}
}

void DCoreTracer::StopRender()
{
	LOG(Error, "Coretracer.StopRender");
	mCamera.StopRender();
	if (threadHandle)
	{
		CloseHandle(threadHandle);
		threadHandle = NULL;
	}
}

__int64 DCoreTracer::GetRayCount()
{
	LOG(Error, "Coretracer.GetRayCount");
	return mScene.mRayCount;
}

__int64 DCoreTracer::GetKDCount()
{
	LOG(Error, "Coretracer.GetKDCount");
	return mScene.mKDTree.mRecursions;
}

__int64 DCoreTracer::GetIntersectionCount()
{
	LOG(Error, "Coretracer.GetIntersectionCount");
	return mScene.mKDTree.mIntersections;
}

__int64 DCoreTracer::GetPixelCount()
{
	LOG(Error, "Coretracer.GetPixelCount");
	return mCamera.mPixelCount;
}

__int64 DCoreTracer::GetPatchSamples()
{
	LOG(Error, "Coretracer.GetPatchSamples");
	return mCamera.mPatchSampleCount;
}
volatile unsigned int g_StartedRender=0;

/****************************
        DLL INTERFACE
****************************/
void InitialiseRender(const char* description)
{
	LOG(Error, "DLL.InitialiseRender");
	if (g_StartedRender==1)
		return;

	delete g_CoreTracer;
	g_CoreTracer = new DCoreTracer();
	g_CoreTracer->BuildFromXml(description);

	float* buffer = new float[g_CoreTracer->GetWidth() * 3 * g_CoreTracer->GetHeight()];
	g_CoreTracer->SetBuffer(buffer);
}

void SetCamera(const char* description)
{
	LOG(Error, "DLL.SetCamera");
	if (g_StartedRender==1)
		return;

	g_CoreTracer->BuildCamera(description);
}

void SetViewport(const char* description)
{
	LOG(Error, "DLL.SetViewport");
	if (g_StartedRender==1)
		return;

	g_CoreTracer->BuildViewport(description);
}

void StartRender()
{
	LOG(Error, "DLL.StartRender");
	int val = InterlockedCompareExchange(&g_StartedRender, 1, 0);
	if (val==1)
		return;

	g_CoreTracer->StartRender(false);
}

void StopRender()
{
	LOG(Error, "DLL.StopRender");
	if (g_CoreTracer)
		g_CoreTracer->StopRender();

	g_StartedRender = 0;
}

void CopyBuffer(float* buffer)
{
	LOG(Error, "DLL.CopyBuffer");
	if (g_CoreTracer)
		g_CoreTracer->CopyBuffer(buffer);
}

void SyncRender(float* buffer)
{
	LOG(Error, "DLL.SyncRender");
	int val = InterlockedCompareExchange(&g_StartedRender, 1, 0);
	if (val==1)
		return;

	g_CoreTracer->StartRender(true);

	if (g_CoreTracer)
		g_CoreTracer->CopyBuffer(buffer);
	
	g_StartedRender = 0;
}

void GetDepth(float& depth, int x, int y)
{
	LOG(Error, "DLL.GetDepth");

	depth = g_CoreTracer->GetDepth(x, y, g_StartedRender>0);
}

int GetDistanceSchemaLength()
{
	return DFuncFactory::Get().GetSchemaLength();
}

void GetDistanceSchema(char* schema)
{
	memcpy(schema, DFuncFactory::Get().GetSchema(), DFuncFactory::Get().GetSchemaLength());
}

void PostProcess(float* targetBuffer, float* sourceBuffer, double maxValue, int iterations, float* kernel, float boostPower, float targetWeighting, float sourceWeighting, int width, int height)
{
	g_CoreTracer->PostProcess(targetBuffer, sourceBuffer, maxValue, iterations, kernel, boostPower, targetWeighting, sourceWeighting, width, height);
}

void GaussianBlur(float* targetBuffer, float* sourceBuffer, double maxValue, int size, float boostPower, float targetWeighting, float sourceWeighting, int width, int height)
{
	g_CoreTracer->GaussianBlur(targetBuffer, sourceBuffer, maxValue, size, boostPower, targetWeighting, sourceWeighting, width, height);
}


__int64 GetRayCount()
{
	LOG(Error, "DLL.GetRayCount");
	if (g_CoreTracer)
		return g_CoreTracer->GetRayCount();
	else
		return 0;
}

__int64 GetKDCount()
{
	LOG(Error, "DLL.GetKDCount");
	if (g_CoreTracer)
		return g_CoreTracer->GetKDCount();
	else
		return 0;
}

__int64 GetIntersectionCount()
{
	LOG(Error, "DLL.GetIntersectionCount");
	if (g_CoreTracer)
		return g_CoreTracer->GetIntersectionCount();
	else
		return 0;
}

__int64 GetPixels()
{
	LOG(Error, "DLL.GetPixels");
	if (g_CoreTracer)
		return g_CoreTracer->GetPixelCount();
	else
		return 0;
}

__int64 GetPatchSamples()
{
	LOG(Error, "DLL.GetPatchSamples");
	if (g_CoreTracer)
		return g_CoreTracer->GetPatchSamples();
	else
		return 0;
}
/*
bool RenderPatch(float* buffer, const char* description, int x, int y)
{
	LOG(Error, "DLL.RenderPatch");
	DCoreTracer CoreTracer;

	CoreTracer.BuildFromXml(description);
	CoreTracer.SetBuffer(buffer);
	CoreTracer.RenderPatch(x, y);

	return true;
}*/

LARGE_INTEGER gStart, gFreq;

void StartRayTracer()
{
	LOG(Error, "DLL.StartRayTracer");
	std::string scene;
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"-9.25, 10, -10.5\" target=\"0, 0, 0\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"15\" apertureSize=\"1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE>\r\n  <LIGHT type=\"0\" position=\"-23, 20, -22.25\" colour=\"500.1, 500.1, 500.1\" />\r\n  <OBJECT type=\"4\" transform=\"0, 0, 0, 10, 10, 10, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"0.75, 20, -14.25\" target=\"0, 0, -1.25\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE>\r\n  <LIGHT type=\"0\" colour=\"1000.1, 1000.1, 1000.1\" position=\"100,100,100\" />\r\n  <OBJECT type=\"1\" transform=\"0, 0, 0, 40, 40, 40, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"0, 4, 13.5, 3.6, 3.6, 3.6, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"7.93510090594839, 4, 10.9217294240618, 3.6, 3.6, 3.6, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"12.8392629699846, 4, 4.17172942406179, 3.6, 3.6, 3.6, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"12.8392629699846, 4, -4.17172942406179, 3.6, 3.6, 3.6, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"7.93510090594839, 4, -10.9217294240618, 3.6, 3.6, 3.6, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"1.65321857766021E-15, 4, -13.5, 3.6, 3.6, 3.6, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-7.93510090594839, 4, -10.9217294240618, 3.6, 3.6, 3.6, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-12.8392629699846, 4, -4.17172942406179, 3.6, 3.6, 3.6, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-12.8392629699846, 4, 4.17172942406179, 3.6, 3.6, 3.6, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-7.9351009059484, 4, 10.9217294240618, 3.6, 3.6, 3.6, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"2\" transform=\"0, 5, 0, 10, 10, 10, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"0.75, 20, -41.75\" target=\"2.75, 0, -9.25\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"25\" apertureSize=\"1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE>\r\n  <LIGHT type=\"0\" position=\"0, 20, -5\" colour=\"100.1, 100.1, 100.1\" />\r\n  <OBJECT type=\"3\" transform=\"0, 0, 0, 50, 50, 50, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"0, 25, 25, 50, 50, 50, 0, 1.570796325, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"0, 50, 0, 25, 25, 25, 0, 3.14159265, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"0, 20, -20\" target=\"0, 0, 0\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE>\r\n  <LIGHT type=\"0\" position=\"10, 20, -10\" colour=\"100.1, 100.1, 100.1\" />\r\n  <OBJECT type=\"4\" iterations=\"3\" pattern=\"1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0\" transform=\"0, 5, 0, 10, 10, 10, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"1\" transform=\"0, 0, 0, 25, 25, 25, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"-8.25, 10, -7.75\" target=\"0, 0, 0\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"10\" apertureSize=\"1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE>\r\n  <LIGHT type=\"0\" colour=\"100.5,100.5,100.5\" position=\"10, 20, -10\" />\r\n  <OBJECT type=\"3\" transform=\"0, 0, 0, 50, 50, 50, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"0, 25, 25, 50, 50, 50, 0, 1.570796325, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"0, 50, 0, 50, 50, 50, 0, 3.14159265, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"-25, 25, 0, 50, 50, 50, 0, 1.570796325, 1.570796325\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"25, 25, 0, 50, 50, 50, 0, 1.570796325, -1.570796325\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0, 1, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"4\" iterations=\"2\" pattern=\"1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1\" transform=\"0, 5, 0, 10, 10, 10, 0.7853981625, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"70\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0.8, 1, 0.8\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"0.75, 8, -9.25\" target=\"0, 0, 0\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE>\r\n  <LIGHT type=\"4\" colour=\"100, 100, 100\" position=\"-12.75, 20, -6.5\" radius=\"10\" samples=\"256\" />\r\n  <OBJECT type=\"1\" transform=\"0, 0, 0, 25, 25, 25, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"1\" colour1=\"0.01, 0.01, 0.01\" colour2=\"1, 1, 1\" scale=\"1\" ratio=\"0.5\" checkerboard=\"true\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"0, 0, 0, 5, 5, 5, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"0, 20, -20\" target=\"0, 0, 0\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE>\r\n  <OBJECT type=\"1\" transform=\"0, 0, 0, 25, 25, 25, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <BACKGROUND type=\"1\" backgroundColour=\"220.2, 0, 0\" />\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"0, 10, -10\" target=\"0, 0, 10\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE>\r\n  <LIGHT type=\"0\" position=\"0, 20, 0\" colour=\"100.1, 100.1, 100.1\" />\r\n  <OBJECT type=\"1\" transform=\"0, 0, 0, 25, 25, 25, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"0, 5, 13.5, 9, 9, 0.9, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"7.93510090594839, 5, 10.9217294240618, 8.1, 8.5, 1.3, 0.1570796325, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"12.8392629699846, 5, 4.17172942406179, 7.2, 8, 1.7, 0.314159265, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"12.8392629699846, 5, -4.17172942406179, 6.3, 7.5, 2.1, 0.4712388975, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"7.93510090594839, 5, -10.9217294240618, 5.4, 7, 2.5, 0.62831853, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"1.65321857766021E-15, 5, -13.5, 4.5, 6.5, 2.9, 0.7853981625, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-7.93510090594839, 5, -10.9217294240618, 3.6, 6, 3.3, 0.942477795, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-12.8392629699846, 5, -4.17172942406179, 2.7, 5.5, 3.7, 1.0995574275, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-12.8392629699846, 5, 4.17172942406179, 1.8, 5, 4.1, 1.25663706, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-7.9351009059484, 5, 10.9217294240618, 0.9, 4.5, 4.5, 1.4137166925, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <BACKGROUND type=\"0\" backgroundColour=\"0, 0, 0\" />\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"1.25, 10, -3.75\" target=\"2.5, 10, 10\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE>\r\n  <LIGHT type=\"0\" position=\"0, 20, 0\" colour=\"100.1, 100.1, 100.1\" />\r\n  <OBJECT type=\"1\" transform=\"0, 0, 0, 25, 25, 25, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"0, 5, 13.5, 9, 9, 0.9, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"7.93510090594839, 5, 10.9217294240618, 8.1, 8.5, 1.3, 0.1570796325, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"12.8392629699846, 5, 4.17172942406179, 7.2, 8, 1.7, 0.314159265, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"12.8392629699846, 5, -4.17172942406179, 6.3, 7.5, 2.1, 0.4712388975, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"7.93510090594839, 5, -10.9217294240618, 5.4, 7, 2.5, 0.62831853, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"1.65321857766021E-15, 5, -13.5, 4.5, 6.5, 2.9, 0.7853981625, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-7.93510090594839, 5, -10.9217294240618, 3.6, 6, 3.3, 0.942477795, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-12.8392629699846, 5, -4.17172942406179, 2.7, 5.5, 3.7, 1.0995574275, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-12.8392629699846, 5, 4.17172942406179, 1.8, 5, 4.1, 1.25663706, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-7.9351009059484, 5, 10.9217294240618, 0.9, 4.5, 4.5, 1.4137166925, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <BACKGROUND type=\"0\" backgroundColour=\"0, 0, 0\" />\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"0, 5, 1.75\" target=\"1.75, 0, 17.5\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE>\r\n  <LIGHT type=\"0\" position=\"2, 20, -2.75\" colour=\"100.1, 100.1, 100.1\" />\r\n  <OBJECT type=\"1\" transform=\"0, 0, 0, 25, 25, 25, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-2, 0.1, 9.69999998807907, 3.33333333333333, 3.33333333333333, 0.0666666666666667, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"6.66025403784439, 0.1, -5.30000001192093, 3.33333333333333, 3.33333333333333, 0.366666666666667, -2.0943951, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-10.6602540378444, 0.1, -5.30000001192093, 3.33333333333333, 3.33333333333333, 0.666666666666667, -4.1887902, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <BACKGROUND type=\"0\" backgroundColour=\"0, 0, 0\" />\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"5.25, 12, -16.25\" target=\"1.5, 0, 1.75\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"true\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"256\" maxSamples=\"256\" /><SCENE>\r\n  <OBJECT type=\"0\" transform=\"0, 5, 0, 4, 4, 4, 0, 0, 0\" ignoreWhileLighting=\"true\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"6.25, 6.25, 6.25\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <LIGHT type=\"4\" colour=\"100, 100, 100\" position=\"0, 5, 0\" radius=\"4\" samples=\"1\" />\r\n  <OBJECT type=\"1\" transform=\"0, 0, 0, 25, 25, 25, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL shininess=\"0.96\" specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"1\" colour1=\"0.05, 0.07, 0.08\" colour2=\"0.65, 0.65, 0.65\" scale=\"1\" ratio=\"0.9\" checkerboard=\"false\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"0, 4, 9, 1.8, 4.5, 0.45, 5.65486677, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"70\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.01, 0.7, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0.8, 1, 0.8\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"5.29006727063226, 4, 7.28115294937453, 1.8, 4.5, 0.45, 5.02654824, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"70\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.01, 0.7, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0.8, 1, 0.8\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"8.55950864665638, 4, 2.78115294937453, 1.8, 4.5, 0.45, 4.39822971, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"70\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.01, 0.7, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0.8, 1, 0.8\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"8.55950864665638, 4, -2.78115294937453, 1.8, 4.5, 0.45, 3.76991118, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"70\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.01, 0.7, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0.8, 1, 0.8\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"5.29006727063226, 4, -7.28115294937453, 1.8, 4.5, 0.45, 3.14159265, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"70\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.01, 0.7, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0.8, 1, 0.8\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"1.10214571844014E-15, 4, -9, 1.8, 4.5, 0.45, 2.51327412, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"70\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.01, 0.7, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0.8, 1, 0.8\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-5.29006727063226, 4, -7.28115294937453, 1.8, 4.5, 0.45, 1.88495559, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"70\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.01, 0.7, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0.8, 1, 0.8\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-8.55950864665638, 4, -2.78115294937453, 1.8, 4.5, 0.45, 1.25663706, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"70\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.01, 0.7, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0.8, 1, 0.8\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-8.55950864665638, 4, 2.78115294937453, 1.8, 4.5, 0.45, 0.62831853, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"70\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.01, 0.7, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0.8, 1, 0.8\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-5.29006727063227, 4, 7.28115294937452, 1.8, 4.5, 0.45, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"70\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.01, 0.7, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0.8, 1, 0.8\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <BACKGROUND type=\"0\" backgroundColour=\"0, 0, 0\" />\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"-1.5, 5, -15.75\" target=\"0, 2, 0\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE>\r\n  <OBJECT type=\"0\" transform=\"-5, 5, 0, 5, 5, 5, 0, 0, 0\" ignoreWhileLighting=\"true\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"4, 4, 4\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <LIGHT type=\"4\" colour=\"50, 50, 50\" position=\"-5, 5, 0\" radius=\"5\" samples=\"64\" />\r\n  <OBJECT type=\"1\" transform=\"0, 0, 0, 25, 25, 25, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"5, 5, 0, 5, 5, 5, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <BACKGROUND type=\"0\" backgroundColour=\"0, 0, 0\" />\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"0, 20, -49.5\" target=\"0, 0, 0\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"true\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"128\" maxSamples=\"128\" /><SCENE>\r\n  <LIGHT type=\"0\" position=\"0, 20, 0\" colour=\"100.1, 100.1, 100.1\" />\r\n  <OBJECT type=\"3\" transform=\"0, 0, 0, 50, 50, 50, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"0, 25, 25, 50, 50, 50, 0, 1.570796325, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"0, 50, 0, 50, 50, 50, 0, 3.14159265, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"-25, 25, 0, 50, 50, 50, 0, 1.570796325, 1.570796325\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.63, 0.06, 0.04\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"25, 25, 0, 50, 50, 50, 0, 1.570796325, -1.570796325\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.15, 0.48, 0.09\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <BACKGROUND type=\"0\" backgroundColour=\"0, 0, 0\" />\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"0, 20, -20\" target=\"0, 0, 0\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE>\r\n  <OBJECT type=\"0\" transform=\"0, 20, 0, 1, 1, 1, 0, 0, 0\" ignoreWhileLighting=\"true\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"100, 100, 100\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <LIGHT type=\"4\" colour=\"100, 100, 100\" position=\"0, 20, 0\" radius=\"0.01\" samples=\"1\" />\r\n  <OBJECT type=\"3\" transform=\"0, 0, 0, 50, 50, 50, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"0, 25, 25, 50, 50, 50, 0, 1.570796325, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"0, 50, 0, 50, 50, 50, 0, 3.14159265, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"-25, 25, 0, 50, 50, 50, 0, 1.570796325, 1.570796325\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.63, 0.06, 0.04\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"25, 25, 0, 50, 50, 50, 0, 1.570796325, -1.570796325\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.15, 0.48, 0.09\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <BACKGROUND type=\"0\" backgroundColour=\"0, 0, 0\" />\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"-0.25, 20, -27.5\" target=\"0, 15, 0\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"true\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"16\" maxSamples=\"16\" /><SCENE pathTracer=\"1\">\r\n  <OBJECT type=\"0\" transform=\"-1.75, 40, -3.75, 2, 2, 2, 0, 0, 0\" ignoreWhileLighting=\"true\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0.4444444, 0.4444444, 0.4444444\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <LIGHT type=\"4\" colour=\"100, 100, 100\" position=\"-1.75, 40, -3.75\" radius=\"2\" samples=\"1\" />\r\n  <OBJECT type=\"3\" transform=\"0, 0, 0, 50, 50, 50, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"0, 25, 25, 50, 50, 50, 0, 1.570796325, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"0, 50, 0, 50, 50, 50, 0, 3.14159265, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"-25, 25, 0, 50, 50, 50, 0, 1.570796325, 1.570796325\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.63, 0.06, 0.04\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"25, 25, 0, 50, 50, 50, 0, 1.570796325, -1.570796325\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.15, 0.48, 0.09\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-13.25, 10, 13.5, 10, 10, 10, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"70\" opacity=\"1\" density=\"1\" shininess=\"0.95\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.01, 0.7, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"9.5, 5, 13.75, 5, 5, 5, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"70\" opacity=\"1\" density=\"1\" shininess=\"0.9\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.01, 0.7, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0.8, 1, 0.8\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <BACKGROUND type=\"0\" backgroundColour=\"0, 0, 0\" />\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"-0.25, 20, -27.5\" target=\"0, 15, 0\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"true\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"16\" maxSamples=\"16\" /><SCENE pathTracer=\"1\">\r\n  <LIGHT type=\"0\" colour=\"200, 200, 200\" position=\"-1.75, 40, -3.75\" radius=\"15\" samples=\"1\" />\r\n  <OBJECT type=\"3\" transform=\"0, 0, 0, 50, 50, 50, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"0, 25, 25, 50, 50, 50, 0, 1.570796325, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"0, 50, 0, 50, 50, 50, 0, 3.14159265, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"-25, 25, 0, 50, 50, 50, 0, 1.570796325, 1.570796325\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.63, 0.06, 0.04\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"25, 25, 0, 50, 50, 50, 0, 1.570796325, -1.570796325\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.15, 0.48, 0.09\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"-13.25, 10, 13.5, 10, 10, 10, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"70\" opacity=\"1\" density=\"1\" shininess=\"0.95\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.01, 0.7, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" transform=\"9.5, 5, 13.75, 5, 5, 5, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"70\" opacity=\"1\" density=\"1\" shininess=\"0.9\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.01, 0.7, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0.8, 1, 0.8\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <BACKGROUND type=\"0\" backgroundColour=\"0, 0, 0\" />\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=750 height=500/><CAMERA from=\"-0.25, 25, -46.25\" target=\"0, 25, 0\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"true\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"5\" maxSamples=\"5\" /><SCENE pathTracer=\"1\">\r\n  <OBJECT type=\"0\" transform=\"0, 50, 20, 8, 8, 8, 0, 0, 0\" ignoreWhileLighting=\"true\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"1.5625, 1.328125, 0.671875\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <LIGHT type=\"4\" colour=\"100, 85, 43\" position=\"0, 50, 20\" radius=\"8\" samples=\"1\" />\r\n  <OBJECT type=\"3\" transform=\"0, 0, 20, 50, 50, 50, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"0, 25, 45, 50, 50, 50, 0, 1.570796325, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"0, 50, 20, 50, 50, 50, 0, 3.14159265, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"-25, 25, 20, 50, 50, 50, 0, 1.570796325, 1.570796325\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.63, 0.06, 0.04\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"3\" transform=\"25, 25, 20, 50, 50, 50, 0, 1.570796325, -1.570796325\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.15, 0.48, 0.09\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"2\" transform=\"8.5, 7.5, 17.75, 15, 15, 15, -0.4363323125, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"2\" transform=\"-9, 12.5, 32.5, 15, 25, 15, 0.34906585, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"0.76, 0.75, 0.5\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <BACKGROUND type=\"0\" backgroundColour=\"0, 0, 0\" />\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=271 height=271/>\r\n<CAMERA from=\"0,0,-10\" target=\"0,0,0\" up=\"0,1,0\" fov=\"12\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"5\" maxSamples=\"5\" />\r\n<SCENE pathtracer=\"0\">\r\n\t<LIGHT type=\"0\" position=\"10, 10, -10\" colour=\"50.0, 50.0, 50.0\"/>\r\n\t<OBJECT type=0 transform=\"0, 0, 0, 2, 2, 2, 0, 0, 0\" ignoreWhileLighting=\"false\"><MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n  <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n  <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n  <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n  <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.3, 0.3, 0.3\" />\r\n  <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n</MATERIAL>\r\n\t</OBJECT>\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"0, 20, -20\" target=\"0, 0, 0\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE pathTracer=\"0\">\r\n  <LIGHT type=\"0\" position=\"15, 20, -15.25\" colour=\"100.099998474121, 100.099998474121, 100.099998474121\" />\r\n  <OBJECT type=\"1\" transform=\"0, 0, 0, 25, 25, 25, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1.0\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.300000011920929, 0.300000011920929, 0.300000011920929\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"2\" transform=\"0, 0, 0, 1, 1, 1, 0, 0, 0\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.300000011920929, 0.300000011920929, 0.300000011920929\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <BACKGROUND type=\"0\" backgroundColour=\"0, 0, 0\" />\r\n</SCENE>";
//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"0, 20, -20\" target=\"0, 0, 0\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE pathTracer=\"0\">\r\n  <LIGHT type=\"0\" position=\"-17.5, 20, -16.75\" colour=\"100.099998474121, 100.099998474121, 100.099998474121\" />\r\n  <OBJECT type=\"1\" position=\"0, 0, 0\" scale=\"25, 25, 25\" rotation=\"1, 0, 0, 0, 1, 0, 0, 0, 1\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.300000011920929, 0.300000011920929, 0.300000011920929\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <BACKGROUND type=\"0\" backgroundColour=\"0, 0, 0\" />\r\n</SCENE>";
	WCHAR my_documents[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

	if (result != S_OK)
		std::cout << "Error: " << result << "\n";

	std::wstringstream ss;
	
//	ss << my_documents << L"\\Wooscripter\\XML\\simplebox.xml";
//	ss << my_documents << L"\\Wooscripter\\XML\\pedestal.xml";
	ss << my_documents << L"\\Woofractal\\XML\\testde.xml";
//	ss << my_documents << L"\\Wooscripter\\XML\\tetraraw.xml";
//	ss << my_documents << L"\\Wooscripter\\XML\\diffuseFunction2.xml";

	std::ifstream sceneDesc;
	sceneDesc.open(ss.str());

	sceneDesc.seekg(0, std::ios::end);
	scene.resize((unsigned int)sceneDesc.tellg());
	sceneDesc.seekg(0, std::ios::beg);
	sceneDesc.read(&scene[0], scene.size());
	sceneDesc.close();


	int widx = scene.find("width=");
	gWidth = atoi(scene.substr(widx+6, 4).c_str());
	int hidx = scene.find("height=");
	gHeight = atoi(scene.substr(hidx+7, 4).c_str());

//	gWidth = 450;
//	gHeight = 300;
//	scene = "\r\n<VIEWPORT width=450 height=300/><CAMERA from=\"-10, 10, -20\" target=\"0, 0, 0\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE pathTracer=\"0\">\r\n  <LIGHT type=\"0\" position=\"0, 20, 0\" colour=\"100.099998474121, 100.099998474121, 100.099998474121\" />\r\n  <OBJECT type=\"2\" position=\"0, 5, 0\" scale=\"10, 10, 10\" rotation=\"1, 0, 0, 0, 1, 0, 0, 0, 1\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 0.5, 0\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.300000011920929, 0.300000011920929, 0.300000011920929\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <BACKGROUND type=\"0\" backgroundColour=\"0.06, 0.05, 0.04\" />\r\n</SCENE>";

	gBuffer = new float[gWidth*gHeight*3];
	gDestBuffer = new float[gWidth*gHeight*3];

//				scene = "\r\n<VIEWPORT width=470 height=300/><CAMERA from=\"9.25, 5, 6\" target=\"-7, 0, 0.75\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"false\" focusDepth=\"5\" apertureSize=\"0.1\" minSamples=\"32\" maxSamples=\"128\" /><SCENE pathTracer=\"0\">\r\n  <LIGHT type=\"3\" colour=\"0.1, 0.09, 0.06\" direction=\"1, 1, 1\" area=\"0.05\" />\r\n  <OBJECT type=\"1\" position=\"0, 0, 0\" scale=\"25, 25, 25\" rotation=\"1, 0, 0, 0, 1, 0, 0, 0, 1\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"5\" opacity=\"1\" density=\"1\" shininess=\"1.0\">\r\n      <DIFFUSECOLOUR type=\"1\" colour1=\"0.00999999977648258, 0.00999999977648258, 0.00999999977648258\" colour2=\"1, 1, 1\" scale=\"1\" ratio=\"0.5\" checkerboard=\"true\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.300000011920929, 0.300000011920929, 0.300000011920929\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <OBJECT type=\"0\" position=\"-11.5, 5, 3\" scale=\"5, 5, 5\" rotation=\"1, 0, 0, 0, 1, 0, 0, 0, 1\" ignoreWhileLighting=\"false\">\r\n    <MATERIAL specularPower=\"50\" opacity=\"1\" density=\"1\" shininess=\"1.0\">\r\n      <DIFFUSECOLOUR type=\"0\" colour=\"1, 1, 1\" />\r\n      <SPECULARCOLOUR type=\"0\" colour=\"0.5, 0.5, 0.5\" />\r\n      <EMISSIVECOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n      <REFLECTIVITYCOLOUR type=\"0\" colour=\"0.300000011920929, 0.300000011920929, 0.300000011920929\" />\r\n      <ABSORPTIONCOLOUR type=\"0\" colour=\"0, 0, 0\" />\r\n    </MATERIAL>\r\n  </OBJECT>\r\n  <BACKGROUND type=\"0\" backgroundColour=\"0.07, 0.09, 01\" />\r\n</SCENE>";

	InitialiseRender(scene.c_str());
	//StartRender();
	//StartRender();
	//SyncRender(gBuffer);

	QueryPerformanceCounter(&gStart);
	QueryPerformanceFrequency(&gFreq);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
#ifdef _DEBUG
_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//_CrtSetBreakAlloc( 189638 );
#endif  

	// now it's time to kick off the raytracer
	StartRayTracer();

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DEBUGEXETEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DEBUGEXETEST));

	// Main message loop:
	while (!gDone && GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	StopRender();
	delete g_CoreTracer;
	delete [] gBuffer;
	delete [] gDestBuffer;

	return (int) msg.wParam;
}




//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DEBUGEXETEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DEBUGEXETEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

HDC        hdcMem = NULL;
DWORD*     pBits = NULL;

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	BITMAPINFO bmp   = { 0 };
	HBITMAP    hBmp  = NULL;
 
	bmp.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bmp.bmiHeader.biWidth       = gWidth;
	bmp.bmiHeader.biHeight      = gHeight;
	bmp.bmiHeader.biPlanes      = 1;
	bmp.bmiHeader.biBitCount    = 32;
	bmp.bmiHeader.biCompression = BI_RGB;
 
	hBmp = CreateDIBSection(NULL, &bmp, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
	hdcMem = CreateCompatibleDC(NULL);
	SelectObject(hdcMem, hBmp);

	hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

			StartRender();
	// start the update animation timer
	SetTimer(hWnd, 0, 30, 0);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
bool bStopRender = false;
LARGE_INTEGER gNow;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_TIMER:
		{
//			SyncRender(gBuffer);
			CopyBuffer(gBuffer);
			GaussianBlur(gDestBuffer, gBuffer, 2, 10, 10, 0.5, 0.5, gWidth, gHeight);
			memcpy(gBuffer, gDestBuffer, sizeof(float)*3*gWidth*gHeight);
			if (pBits)
			{
				for (int y = 0; y < gHeight; ++y)
					for (int x = 0; x < gWidth; ++x)
					{
						int r = int(gBuffer[(x + y*gWidth) * 3 + 0]*255);
						int g = int(gBuffer[(x + y*gWidth) * 3 + 1]*255);
						int b = int(gBuffer[(x + y*gWidth) * 3 + 2]*255);
						r = min(r, 255);
						g = min(g, 255);
						b = min(b, 255);
						pBits[(((gHeight-1)-y) * gWidth) + x] = RGB(b, g, r);
					}
			}
			InvalidateRect(hWnd, 0, false);
			SetTimer(0,0,30,0);
			float x, z;
			static float theta=0;
			x = 10*sinf(theta);
			z = 10*cosf(theta);
			theta += 0.001f;
			std::ostringstream cam;
			cam << "<CAMERA from=\"";
			cam << x;
			cam << ", 4, ";
			cam << z;
			cam << "\" target=\"0, 4, 0\" up=\"0, 1, 0\" fov=\"40\" dofEnabled=\"false\" aaEnabled=\"true\" focusDepth=\"1\" apertureSize=\"1\" minSamples=\"1\" maxSamples=\"1\" />";
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps = { 0 };
 
			BeginPaint(hWnd, &ps);
 
			if (hdcMem)
				BitBlt(ps.hdc, 0, 0, gWidth, gHeight, hdcMem, 0, 0, SRCCOPY);

			gNow;
			if (!bStopRender)
				QueryPerformanceCounter(&gNow);
			double time = double(gNow.QuadPart - gStart.QuadPart) / double(gFreq.QuadPart);

			WCHAR dest[256];
			__int64 rayCount = GetRayCount();
			__int64 kdCount = GetKDCount();
			__int64 intersectionCount = GetIntersectionCount();
			__int64 pixelCount = GetPixels();
			__int64 patchSamples = GetPatchSamples();
//			if (patchSamples>3000000)
//			{
//				bStopRender = true;
//				StopRender();
//			}
			__int64 pixels = (gWidth*gHeight);

			int y=20;
			swprintf(dest, 256, L"Time : %f\r\n", time);
			TextOut(ps.hdc, gWidth+20, y+=20, dest, wcslen(dest));
			swprintf(dest, 256, L"Samples : %d\r\n", patchSamples);
			TextOut(ps.hdc, gWidth+20, y+=20, dest, wcslen(dest));
			swprintf(dest, 256, L"Rays : %d\r\n", rayCount);
			TextOut(ps.hdc, gWidth+20, y+=20, dest, wcslen(dest));
			swprintf(dest, 256, L"KD Recursions : %d\r\n", kdCount);
			TextOut(ps.hdc, gWidth+20, y+=20, dest, wcslen(dest));
			swprintf(dest, 256, L"Obj Intersections: %d\r\n", intersectionCount);
			TextOut(ps.hdc, gWidth+20, y+=20, dest, wcslen(dest));
			swprintf(dest, 256, L"Obj / Rays : %f\r\n", rayCount>0 ? double(intersectionCount)/double(rayCount) : 0);
			TextOut(ps.hdc, gWidth+20, y+=20, dest, wcslen(dest));
			swprintf(dest, 256, L"Rays / sec : %f\r\n", double(rayCount)/time);
			TextOut(ps.hdc, gWidth+20, y+=20, dest, wcslen(dest));
			swprintf(dest, 256, L"Rays / sample : %f\r\n", double(rayCount)/patchSamples);
			TextOut(ps.hdc, gWidth+20, y+=20, dest, wcslen(dest));
			swprintf(dest, 256, L"Samples / pixel : %f\r\n", double(patchSamples)/pixels);
			TextOut(ps.hdc, gWidth+20, y+=20, dest, wcslen(dest));
			swprintf(dest, 256, L"Samples / pixel sampled : %f\r\n", double(patchSamples)/pixelCount);
			TextOut(ps.hdc, gWidth+20, y+=20, dest, wcslen(dest));
			swprintf(dest, 256, L"kd / ray : %f\r\n", double(kdCount)/rayCount);
			TextOut(ps.hdc, gWidth+20, y+=20, dest, wcslen(dest));

			EndPaint(hWnd, &ps);
		}
		break;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
