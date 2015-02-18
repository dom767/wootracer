#pragma once

using namespace System::Threading;
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;

class DColour;

public ref class DProgressReporter abstract
{
public:
	virtual void SetProgress(const float Progress)=0;
};

public ref class DThreadedRenderer
{
public:

	DThreadedRenderer(DProgressReporter^ pProgressReporter, int Width, int Height, DColour* pRenderBuffer)
		: m_pProgressReporter(pProgressReporter),
		m_Row(0),
		m_Width(Width),
		m_Height(Height),
		m_pRenderBuffer(pRenderBuffer)
		{}
	~DThreadedRenderer()
		{}

	void Start(int numThreads);
	void Stop();
	int GetNextRow();

private:
	/// Progress reporter to update parent object when profgress changes
	DProgressReporter^ m_pProgressReporter;
	/// Use this to protect access to the worker stack
	ReaderWriterLock^ m_pRWLock;
	/// Render Buffer dimensions
	int m_Height;
	int m_Width;
	int m_Row;
	/// The final render buffer for returning the results in (multi write)
	DColour* m_pRenderBuffer;

};