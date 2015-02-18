// NoiseAnalysis.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "NoiseAnalysis.h"
#include "RandomSequence.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_NOISEANALYSIS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NOISEANALYSIS));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NOISEANALYSIS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_NOISEANALYSIS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

HDC        hdcMem = NULL;
DWORD*     pBits = NULL;

class DGeneratorAnalysis
{
public:
	DGeneratorAnalysis(DRandomSequence::EGenerator generator, int numSequences) : mRandomSequence(NULL), mGeneratorType(generator), mNumSequences(numSequences)
	{
		for (int i=0; i<256; i++)
		{
			mSum[i]=0;
			mTests=0;
		}
	}
	~DGeneratorAnalysis()
	{
		delete mRandomSequence;
	}

	void Generate()
	{
		LARGE_INTEGER start, frequency, end;

		QueryPerformanceCounter(&start);
		QueryPerformanceFrequency(&frequency);

		delete mRandomSequence;
		mRandomSequence = new DRandomSequence(mGeneratorType, mNumSequences, 57612);
		mRandomSequence->GenerateSequences();

		QueryPerformanceCounter(&end);

		mGenerationTime = double(end.QuadPart - start.QuadPart) / double(frequency.QuadPart);
	}

	void TestSequence(int sequence, float* aliasTest, float realResult)
	{
		float accumulator = 0;
		for (int i=0; i<256; i++)
		{
			DVector2 sample = mRandomSequence->GetValue(sequence, i);
			sample.x *= 255.99f;
			sample.y *= 255.99f;
			float value = aliasTest[int(sample.x) + int(sample.y)*256];
			accumulator += value;
			float estimate = accumulator / float(i+1);
			float error = estimate - realResult;
			mSum[i] += fabsf(error);
		}
		mTests++;
	}

	float GetAverageError(int sample)
	{
		return fabsf(mSum[sample]/mTests);
	}

	void RenderNoise(DWORD* bits, DVector2 topCorner)
	{
		for (int q=0; q<1; q++)
			for (int s = 0; s < 256; ++s)
			{
				DVector2 sample = mRandomSequence->GetValue(q, s);
				bits[1024*(int(topCorner.y) + int(sample.y*255.99f))
					+ int(topCorner.x) + int(sample.x*255.99f)] = RGB(0,0,0);
			}
	}

	void RenderAverageError(DWORD* bits, DWORD colour)
	{
		for (int s = 0; s < 256; ++s)
		{
			float averageError = GetAverageError(s);
	
			averageError = averageError * 8;
			if (averageError>1) averageError = 1;

			int val = int(averageError*255.99f);
			if (val>255) val=255;
			bits[((511-val) * 1024) + 256+s] = colour;
		}
	}

	double GetGenerationTime() {return mGenerationTime;}

private:
	DRandomSequence *mRandomSequence;
	DRandomSequence::EGenerator mGeneratorType;
	double mGenerationTime;
	int mNumSequences;
	float mSum[256];
	float mTests;
};

DWORD MakeRGB(int red, int green, int blue)
{
	return RGB(blue, green, red); // DOH!!!
}

class DNoiseAnalysis
{
public:
	static const int mToTest = 256;
	DNoiseAnalysis()
	{
		mGeneratorAnalysis.push_back(new DGeneratorAnalysis(DRandomSequence::Random, mToTest));
		mGeneratorAnalysis.push_back(new DGeneratorAnalysis(DRandomSequence::MostDistant2, mToTest));
		mGeneratorAnalysis.push_back(new DGeneratorAnalysis(DRandomSequence::MostDistant4, mToTest));
		mGeneratorAnalysis.push_back(new DGeneratorAnalysis(DRandomSequence::Jitter, mToTest));
		mGeneratorAnalysis.push_back(new DGeneratorAnalysis(DRandomSequence::Poisson, mToTest));
		mGeneratorAnalysis.push_back(new DGeneratorAnalysis(DRandomSequence::PoissonJitter, mToTest));
		mGeneratorAnalysis.push_back(new DGeneratorAnalysis(DRandomSequence::HeirarchicalJitter, mToTest));
		mGeneratorAnalysis.push_back(new DGeneratorAnalysis(DRandomSequence::GridJitter, mToTest));
	}
	~DNoiseAnalysis()
	{
		while (mGeneratorAnalysis.size()>0)
		{
			delete mGeneratorAnalysis[mGeneratorAnalysis.size()-1];
			mGeneratorAnalysis.pop_back();
		}
	}

	void Analyse()
	{
		for (unsigned int g=0; g<mGeneratorAnalysis.size(); g++)
		{
			mGeneratorAnalysis[g]->Generate();
		}

		GenerateAliasTest();

		for (unsigned int g=0; g<mGeneratorAnalysis.size(); g++)
		{
			for (int i=0; i<mToTest; i++)
			{
				mGeneratorAnalysis[g]->TestSequence(i, mAliasTest, mRealResult);
			}
		}
	}


	void GenerateAliasTest()
	{
		float total=0;
		float min=100, max=-1;

		for (int y=0; y<256; y++)
			for (int x=0; x<256; x++)
			{
//				float value = float(int(int(7.f*float(x)/256.f)%2 + int(7.f*float(y)/256.f)%2)%2);
/*				float fx = float(x)/256;
				float fy = float(y)/256;
				float value = cosf(sqrtf((fx-0.7f)*(fx-0.7f) + (fy-0.65f)*(fy-0.65f))*10)+1;
				*/
				float fx = float(x)/256;
				float fy = float(y)/256;
				float value = (fx*0.3 + fy*0.8) - 0.4 <0 ? 0 : 1;
				mAliasTest[x+y*256] = value;
				total += value;
				if (value<min) min=value;
				if (value>max) max=value;
			}

		total=0;
		for (int i=0; i<256*256; i++)
		{
			mAliasTest[i] = mAliasTest[i]/max;
			total += mAliasTest[i];
		}

		mRealResult = total/65536.f;
	}

	void RenderNoiseExamples(HWND hWnd, PAINTSTRUCT ps)
	{
		if (pBits)
		{
			for (int y=512; y<1024; y++)
			{
				for (int x=0; x<256*4; x++)
				{
					pBits[x+y*1024] = RGB(255, 255, 255);
				}
			}

			for (unsigned int g=0; g<mGeneratorAnalysis.size(); g++)
			{
				mGeneratorAnalysis[g]->RenderNoise(pBits, DVector2(float(g%4*256), float(768-256*(g/4))));
			}
		}
	}

	void RenderResults(HWND hWnd, PAINTSTRUCT ps)
	{
		if (pBits)
		{
			for (int y = 0; y < 256; ++y)
				for (int x = 0; x < 256; ++x)
				{
					int val = int(mAliasTest[x+y*256]*255.99f);
					if (val>255) val=255;
					int r = int(val);
					int g = int(val);
					int b = int(val);
					pBits[((511-y) * 1024) + x] = RGB(b, g, r);
				}

			for (int y = 0; y < 256; ++y)
				for (int x = 0; x < 256; ++x)
				{
					pBits[((511-y) * 1024) + 256 + x] = RGB(255, 255, 255);
				}

			for (unsigned int g=0; g<mGeneratorAnalysis.size(); g++)
			{
				DWORD col;
				switch(g)
				{
				case 0: col = MakeRGB(255,0,0);break;
				case 1: col = MakeRGB(0,255,0);break;
				case 2: col = MakeRGB(0,0,255);break;
				case 3: col = MakeRGB(255,128,0);break;
				case 4: col = MakeRGB(0,128,255);break;
				case 5: col = MakeRGB(0,255,128);break;
				case 6: col = MakeRGB(0,0,0);break;
				case 7: col = MakeRGB(0,255,255);break;
				}
				mGeneratorAnalysis[g]->RenderAverageError(pBits, col);
			}
		}
//		InvalidateRect(hWnd, 0, false);

		if (hdcMem)
			BitBlt(ps.hdc, 0, 0, 1024, 1024, hdcMem, 0, 0, SRCCOPY);

		WCHAR dest[256];

		swprintf(dest, 256, L"Time");
		TextOut(ps.hdc, 20, 266, dest, wcslen(dest));
		
		swprintf(dest, 256, L"1 sample noise");
		TextOut(ps.hdc, 20, 286, dest, wcslen(dest));
		
		swprintf(dest, 256, L"4 sample noise");
		TextOut(ps.hdc, 20, 306, dest, wcslen(dest));

		swprintf(dest, 256, L"16 sample noise");
		TextOut(ps.hdc, 20, 326, dest, wcslen(dest));

		swprintf(dest, 256, L"64 sample noise");
		TextOut(ps.hdc, 20, 346, dest, wcslen(dest));

		swprintf(dest, 256, L"256 sample noise");
		TextOut(ps.hdc, 20, 366, dest, wcslen(dest));

		for (unsigned int g=0; g<mGeneratorAnalysis.size(); g++)
		{
			swprintf(dest, 256, L"%f", mGeneratorAnalysis[g]->GetGenerationTime());
			TextOut(ps.hdc, 200+g*80, 266, dest, wcslen(dest));
			swprintf(dest, 256, L"%f", mGeneratorAnalysis[g]->GetAverageError(0));
			TextOut(ps.hdc, 200+g*80, 286, dest, wcslen(dest));
			swprintf(dest, 256, L"%f", mGeneratorAnalysis[g]->GetAverageError(3));
			TextOut(ps.hdc, 200+g*80, 306, dest, wcslen(dest));
			swprintf(dest, 256, L"%f", mGeneratorAnalysis[g]->GetAverageError(15));
			TextOut(ps.hdc, 200+g*80, 326, dest, wcslen(dest));
			swprintf(dest, 256, L"%f", mGeneratorAnalysis[g]->GetAverageError(63));
			TextOut(ps.hdc, 200+g*80, 346, dest, wcslen(dest));
			swprintf(dest, 256, L"%f", mGeneratorAnalysis[g]->GetAverageError(255));
			TextOut(ps.hdc, 200+g*80, 366, dest, wcslen(dest));
		}
	}

private:
	std::vector<DGeneratorAnalysis*> mGeneratorAnalysis;
	float mAliasTest[256*256];
	float mRealResult;
};

DNoiseAnalysis NoiseAnalysis;

void AnalyseNoise()
{
	NoiseAnalysis.Analyse();
}

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
	bmp.bmiHeader.biWidth       = 1024;
	bmp.bmiHeader.biHeight      = 1024;
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

   AnalyseNoise();

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

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
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...

		NoiseAnalysis.RenderNoiseExamples(hWnd, ps);
		NoiseAnalysis.RenderResults(hWnd, ps);

		EndPaint(hWnd, &ps);
		break;
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
