#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

class DColour;

namespace Raytracer {

	/// <summary>
	/// Summary for RenderView
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class RenderView : public System::Windows::Forms::Form
	{
	public:
		RenderView(void) : mWidth(400),
			mHeight(300),
			mRendering(false),
			mRenderBuffer(0)
		{
			InitializeComponent();
			InitialiseRenderBuffer();
			clampMultiplier->Text = "100.0";
			exposureSensitivity->Text = "1.0";
		}

		void PrepareRender(int width, int height);
		void TransferFloatToInt();

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~RenderView()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  exposureSensitivity;
	protected: 

	protected: 
	private: System::Windows::Forms::Label^  label1;

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::RadioButton^  radioExposure;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::RadioButton^  radioClamp;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::TextBox^  clampMultiplier;
	private: System::Windows::Forms::Label^  label2;
		bool mRendering;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->exposureSensitivity = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->radioExposure = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->radioClamp = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->clampMultiplier = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->SuspendLayout();
			// 
			// exposureSensitivity
			// 
			this->exposureSensitivity->Location = System::Drawing::Point(6, 38);
			this->exposureSensitivity->Name = L"exposureSensitivity";
			this->exposureSensitivity->Size = System::Drawing::Size(85, 20);
			this->exposureSensitivity->TabIndex = 0;
			this->exposureSensitivity->TextChanged += gcnew System::EventHandler(this, &RenderView::exposureSensitivity_TextChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 19);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(54, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Sensitivity";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->exposureSensitivity);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Location = System::Drawing::Point(12, 88);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(109, 68);
			this->groupBox1->TabIndex = 3;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Exposure";
			// 
			// radioExposure
			// 
			this->radioExposure->AutoSize = true;
			this->radioExposure->Location = System::Drawing::Point(6, 19);
			this->radioExposure->Name = L"radioExposure";
			this->radioExposure->Size = System::Drawing::Size(69, 17);
			this->radioExposure->TabIndex = 4;
			this->radioExposure->Text = L"Exposure";
			this->radioExposure->UseVisualStyleBackColor = true;
			this->radioExposure->CheckedChanged += gcnew System::EventHandler(this, &RenderView::radioExposure_CheckedChanged);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->radioClamp);
			this->groupBox2->Controls->Add(this->radioExposure);
			this->groupBox2->Location = System::Drawing::Point(12, 12);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(109, 70);
			this->groupBox2->TabIndex = 5;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Mode";
			// 
			// radioClamp
			// 
			this->radioClamp->AutoSize = true;
			this->radioClamp->Checked = true;
			this->radioClamp->Location = System::Drawing::Point(6, 42);
			this->radioClamp->Name = L"radioClamp";
			this->radioClamp->Size = System::Drawing::Size(54, 17);
			this->radioClamp->TabIndex = 6;
			this->radioClamp->TabStop = true;
			this->radioClamp->Text = L"Clamp";
			this->radioClamp->UseVisualStyleBackColor = true;
			this->radioClamp->CheckedChanged += gcnew System::EventHandler(this, &RenderView::radioClamp_CheckedChanged);
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->clampMultiplier);
			this->groupBox3->Controls->Add(this->label2);
			this->groupBox3->Location = System::Drawing::Point(12, 162);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(109, 68);
			this->groupBox3->TabIndex = 6;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Clamp";
			// 
			// clampMultiplier
			// 
			this->clampMultiplier->Location = System::Drawing::Point(6, 38);
			this->clampMultiplier->Name = L"clampMultiplier";
			this->clampMultiplier->Size = System::Drawing::Size(85, 20);
			this->clampMultiplier->TabIndex = 0;
			this->clampMultiplier->TextChanged += gcnew System::EventHandler(this, &RenderView::clampMultiplier_TextChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(6, 19);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(48, 13);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Multiplier";
			// 
			// RenderView
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(533, 306);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Name = L"RenderView";
			this->Text = L"RenderView";
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &RenderView::RenderView_Paint);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		void InitialiseRenderBuffer();

		System::Drawing::Bitmap^ mImage;
		DColour* mRenderBuffer;
		int mWidth, mHeight;

	public: DColour* CreateBuffer(int Width, int Height);
	private: System::Void RenderView_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
	
	System::Void radioExposure_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		if (mRendering)
			return;

		if (radioExposure->Checked)
			radioClamp->Checked = false;
		TransferFloatToInt();
		this->Refresh();
	}
	
	System::Void radioClamp_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		if (mRendering)
			return;

		if (radioClamp->Checked)
			radioExposure->Checked = false;
		TransferFloatToInt();
		this->Refresh();
	}
	System::Void exposureSensitivity_TextChanged(System::Object^  sender, System::EventArgs^  e)
	{
		if (mRendering)
			return;

		TransferFloatToInt();
		this->Refresh();
	}
	System::Void clampMultiplier_TextChanged(System::Object^  sender, System::EventArgs^  e)
	{
		if (mRendering)
			return;

		TransferFloatToInt();
		this->Refresh();
	}
};
}
