#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Raytracer {

	/// <summary>
	/// Summary for SceneSpace
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class SceneSpace : public System::Windows::Forms::Form
	{
	public:
		SceneSpace(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

		void UpdateScene()
		{
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~SceneSpace()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TreeView^  treeView;
	protected: 

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
			this->treeView = (gcnew System::Windows::Forms::TreeView());
			this->SuspendLayout();
			// 
			// treeView
			// 
			this->treeView->Location = System::Drawing::Point(11, 10);
			this->treeView->Name = L"treeView";
			this->treeView->Size = System::Drawing::Size(248, 488);
			this->treeView->TabIndex = 0;
			// 
			// SceneSpace
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(268, 507);
			this->Controls->Add(this->treeView);
			this->Name = L"SceneSpace";
			this->Text = L"SceneSpace";
			this->ResumeLayout(false);

		}
#pragma endregion
	};
}
