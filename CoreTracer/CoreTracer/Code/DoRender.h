#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;

namespace Raytracer {

	/// <summary>
	/// Summary for DoRender
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class DoRender : public System::Windows::Forms::Form
	{
	public:
		DoRender(void);
	private: System::Windows::Forms::TextBox^  maximumRecursion;
	public: 
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;


	private: System::Windows::Forms::CheckBox^  aaEnable;
	private: System::Windows::Forms::NumericUpDown^  aaSamples;
	private: System::Windows::Forms::TextBox^  width;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::TextBox^  height;




	public: 
		System::ComponentModel::BackgroundWorker^ backgroundWorker;
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~DoRender()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  startRender;
	private: System::Windows::Forms::Button^  cancel;
	protected: 

	public: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  timeTaken;

	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
		void ThreadJob(Object^ sender, DoWorkEventArgs^ e);
		void ThreadProgressChanged(Object^ sender, ProgressChangedEventArgs^ e);
		void ThreadProcessCompleted(Object^ sender, RunWorkerCompletedEventArgs^ e);
	
#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->startRender = (gcnew System::Windows::Forms::Button());
			this->cancel = (gcnew System::Windows::Forms::Button());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->timeTaken = (gcnew System::Windows::Forms::TextBox());
			this->maximumRecursion = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->aaEnable = (gcnew System::Windows::Forms::CheckBox());
			this->aaSamples = (gcnew System::Windows::Forms::NumericUpDown());
			this->width = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->height = (gcnew System::Windows::Forms::TextBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->aaSamples))->BeginInit();
			this->SuspendLayout();
			// 
			// startRender
			// 
			this->startRender->Location = System::Drawing::Point(12, 200);
			this->startRender->Name = L"startRender";
			this->startRender->Size = System::Drawing::Size(128, 28);
			this->startRender->TabIndex = 0;
			this->startRender->Text = L"Start Render";
			this->startRender->UseVisualStyleBackColor = true;
			this->startRender->Click += gcnew System::EventHandler(this, &DoRender::startRender_Click);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(142, 200);
			this->cancel->Name = L"cancel";
			this->cancel->Size = System::Drawing::Size(128, 28);
			this->cancel->TabIndex = 1;
			this->cancel->Text = L"Cancel";
			this->cancel->UseVisualStyleBackColor = true;
			this->cancel->Click += gcnew System::EventHandler(this, &DoRender::cancel_Click);
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(12, 145);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(258, 23);
			this->progressBar1->TabIndex = 2;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(72, 177);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(64, 13);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Time Taken";
			// 
			// timeTaken
			// 
			this->timeTaken->Location = System::Drawing::Point(142, 174);
			this->timeTaken->Name = L"timeTaken";
			this->timeTaken->Size = System::Drawing::Size(128, 20);
			this->timeTaken->TabIndex = 4;
			// 
			// maximumRecursion
			// 
			this->maximumRecursion->Location = System::Drawing::Point(12, 119);
			this->maximumRecursion->Name = L"maximumRecursion";
			this->maximumRecursion->Size = System::Drawing::Size(49, 20);
			this->maximumRecursion->TabIndex = 5;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(67, 122);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(97, 13);
			this->label2->TabIndex = 6;
			this->label2->Text = L"Maximum recursion";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(67, 61);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(103, 13);
			this->label3->TabIndex = 8;
			this->label3->Text = L"Antialiasing Samples";
			// 
			// aaEnable
			// 
			this->aaEnable->AutoSize = true;
			this->aaEnable->Location = System::Drawing::Point(12, 35);
			this->aaEnable->Name = L"aaEnable";
			this->aaEnable->Size = System::Drawing::Size(115, 17);
			this->aaEnable->TabIndex = 10;
			this->aaEnable->Text = L"Enable Antialiasing";
			this->aaEnable->UseVisualStyleBackColor = true;
			// 
			// aaSamples
			// 
			this->aaSamples->Location = System::Drawing::Point(12, 58);
			this->aaSamples->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {512, 0, 0, 0});
			this->aaSamples->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->aaSamples->Name = L"aaSamples";
			this->aaSamples->ReadOnly = true;
			this->aaSamples->Size = System::Drawing::Size(49, 20);
			this->aaSamples->TabIndex = 11;
			this->aaSamples->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->aaSamples->ValueChanged += gcnew System::EventHandler(this, &DoRender::aaSamples_ValueChanged);
			// 
			// width
			// 
			this->width->Location = System::Drawing::Point(12, 93);
			this->width->Name = L"width";
			this->width->Size = System::Drawing::Size(49, 20);
			this->width->TabIndex = 12;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(67, 96);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(35, 13);
			this->label4->TabIndex = 13;
			this->label4->Text = L"Width";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(176, 96);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(38, 13);
			this->label5->TabIndex = 15;
			this->label5->Text = L"Height";
			// 
			// height
			// 
			this->height->Location = System::Drawing::Point(121, 93);
			this->height->Name = L"height";
			this->height->Size = System::Drawing::Size(49, 20);
			this->height->TabIndex = 14;
			// 
			// DoRender
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(282, 240);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->height);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->width);
			this->Controls->Add(this->aaSamples);
			this->Controls->Add(this->aaEnable);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->maximumRecursion);
			this->Controls->Add(this->timeTaken);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->progressBar1);
			this->Controls->Add(this->cancel);
			this->Controls->Add(this->startRender);
			this->Name = L"DoRender";
			this->Text = L"DoRender";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->aaSamples))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	
private:
	System::Void startRender_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void cancel_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void aaSamples_ValueChanged(System::Object^  sender, System::EventArgs^  e);

public:

	System::Void InitialiseVariables();
};
}
