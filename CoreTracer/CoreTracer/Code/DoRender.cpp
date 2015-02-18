#include "StdAfx.h"
#include "DoRender.h"
#include "Form1.h"
#include "ThreadedRenderer.h"
#include <math.h>

using namespace System::Threading;

namespace Raytracer
{
	DoRender::DoRender(void)
	{
		InitializeComponent();

		// set up the progress bar
		progressBar1->Maximum = 100;
		progressBar1->Minimum = 0;

		width->Text = System::Convert::ToString(DScene::GetScene().GetCanvasWidth());
		height->Text = System::Convert::ToString(DScene::GetScene().GetCanvasHeight());
	}

	public ref class DoRenderProgressReporter : public DProgressReporter
	{
	public:
		DoRenderProgressReporter(BackgroundWorker^ pWorker) : m_pWorker(pWorker) {}
		virtual void SetProgress(const float Progress) override
		{
			m_pWorker->ReportProgress(int(Progress*100.f));
		}
	private:
		BackgroundWorker^ m_pWorker;
	};

	void DoRender::ThreadJob(Object^ Sender, DoWorkEventArgs^ e)
    {
		DScene::GetScene().GetRenderTimer().StartTimer();
		BackgroundWorker^ pWorker = dynamic_cast<BackgroundWorker^>(Sender);
		DProgressReporter^ pProgressReporter = gcnew DoRenderProgressReporter(pWorker);
		DColour* pRenderBuffer = Form1::Get()->m_RenderView->CreateBuffer(DScene::GetScene().GetCanvasWidth(),
			DScene::GetScene().GetCanvasHeight());
		DThreadedRenderer^ pRenderer = gcnew DThreadedRenderer(pProgressReporter, DScene::GetScene().GetCanvasWidth(),
			DScene::GetScene().GetCanvasHeight(), pRenderBuffer);
#ifdef _DEBUG
		pRenderer->Start(1);
#else
		pRenderer->Start(4);
#endif
    }

	void DoRender::ThreadProgressChanged(Object^ /*sender*/, ProgressChangedEventArgs^ e)
	{
		this->progressBar1->Value = e->ProgressPercentage;
	}

	void DoRender::ThreadProcessCompleted(Object^ /*sender*/, RunWorkerCompletedEventArgs^ e)
	{
		this->progressBar1->Value = this->progressBar1->Maximum;
		Form1::Get()->m_RenderView->TransferFloatToInt();
		Form1::Get()->m_RenderView->Refresh();

		timeTaken->Text = System::Convert::ToString(DScene::GetScene().GetRenderTimer().EndTimer());
	}

	System::Void DoRender::startRender_Click(System::Object^  sender, System::EventArgs^  e)
	{
		DScene::GetScene().SetMaximumRecursion(System::Convert::ToInt32(maximumRecursion->Text));
		DScene::GetScene().SetAAEnabled(aaEnable->Checked);
		DScene::GetScene().SetAASamples(System::Convert::ToInt32(aaSamples->Text));
		DScene::GetScene().SetCanvasWidth(System::Convert::ToInt32(width->Text));
		DScene::GetScene().SetCanvasHeight(System::Convert::ToInt32(height->Text));
		Form1::Get()->m_RenderView->PrepareRender(DScene::GetScene().GetCanvasWidth(), DScene::GetScene().GetCanvasHeight());

		backgroundWorker = gcnew System::ComponentModel::BackgroundWorker;
		backgroundWorker->DoWork += gcnew DoWorkEventHandler(this, &DoRender::ThreadJob);
		backgroundWorker->WorkerReportsProgress = true;
		backgroundWorker->ProgressChanged += gcnew ProgressChangedEventHandler( this, &DoRender::ThreadProgressChanged );
		backgroundWorker->RunWorkerCompleted += gcnew RunWorkerCompletedEventHandler( this, &DoRender::ThreadProcessCompleted );

      	backgroundWorker->RunWorkerAsync( 1 );
	}

	System::Void DoRender::cancel_Click(System::Object^  sender, System::EventArgs^  e)
	{
	}

	System::Void DoRender::InitialiseVariables()
	{
		maximumRecursion->Text = System::Convert::ToString(DScene::GetScene().GetMaximumRecursion());
		aaEnable->Checked = DScene::GetScene().GetAAEnabled();
		aaSamples->Text = System::Convert::ToString(DScene::GetScene().GetAASamples());
	}

	System::Void DoRender::aaSamples_ValueChanged(System::Object^  sender, System::EventArgs^  e)
	{
		switch (System::Decimal::ToInt32(aaSamples->Value))
		{
		case 0:
			aaSamples->Value = 1;
			break;
		case 2:
			aaSamples->Value = 4;
			break;
		case 3:
			aaSamples->Value = 1;
			break;
		case 5:
			aaSamples->Value = 16;
			break;
		case 15:
			aaSamples->Value = 4;
			break;
		case 17:
			aaSamples->Value = 64;
			break;
		case 63:
			aaSamples->Value = 16;
			break;
		case 65:
			aaSamples->Value = 256;
			break;
		case 255:
			aaSamples->Value = 64;
			break;
		case 257:
			aaSamples->Value = 256;
			break;
		}
	}

}