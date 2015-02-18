#include "stdafx.h"
#include "ThreadedRenderer.h"
#include "Scene.h"

public ref class DChildRenderer
{
public:
	DChildRenderer(int Row, int Width, int Height, DColour* pRenderBuffer, DThreadedRenderer^ pParent) :
	  m_RowToRender(Row), m_Width(Width), m_Height(Height), m_pRenderBuffer(pRenderBuffer), m_pParent(pParent) {}

	void ThreadStart()
	{
		while (m_RowToRender!=-1)
		{
			DScene::GetScene().RenderRow(m_Width, m_Height, m_RowToRender, m_pRenderBuffer);
			m_RowToRender = m_pParent->GetNextRow();
		}
	}

	int m_RowToRender;
	int m_Width;
	int m_Height;
	DColour* m_pRenderBuffer;
	DThreadedRenderer^ m_pParent;
};

void DThreadedRenderer::Start(int NumThreads)
{
	DScene::GetScene().PrepareRender();

	for (int i=0; i<NumThreads; i++)
	{
		DChildRenderer^ ChildRenderer = gcnew DChildRenderer(GetNextRow(), m_Width, m_Height, m_pRenderBuffer, this);
		ThreadStart^ ThreadDelegate = gcnew ThreadStart(ChildRenderer, &DChildRenderer::ThreadStart);
		Thread^ thread = gcnew Thread(ThreadDelegate);
		thread->Start();
	}

	while (m_Row!=m_Height)
	{
		Thread::Sleep(10);
	}
}

void DThreadedRenderer::Stop()
{
	m_Row = -1;
}

int DThreadedRenderer::GetNextRow()
{
	if (m_Row==m_Height)
	{
		return -1;
	}
	else
	{
		m_pProgressReporter->SetProgress(float(m_Row)/float(m_Height));
		return m_Row++;
	}
}