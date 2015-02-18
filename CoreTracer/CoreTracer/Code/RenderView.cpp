#include "StdAfx.h"
#include "RenderView.h"
#include "Form1.h"
#include "Colour.h"

namespace Raytracer
{
	System::Void RenderView::RenderView_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e)
	{
		DScene& scene = Form1::GetScene();

		e->Graphics->DrawImageUnscaled(mImage, 130, 3);
	}

	void RenderView::InitialiseRenderBuffer()
	{
		mImage = gcnew System::Drawing::Bitmap(mWidth, mHeight);

		System::Drawing::Color colour;
		int x, y;
		for (y=0; y<mHeight; y++)
			for (x=0; x<mWidth; x++)
			{
				if (x%2==0)
					colour = colour.FromArgb(255, 255, 255);
				else
					colour = colour.FromArgb(0, 0, 0);
				mImage->SetPixel(x, y, colour);
			}
	}

	void RenderView::PrepareRender(int width, int height)
	{
		if (width!=mWidth || height!=mHeight)
		{
			int deltax = width-mWidth,
				deltay = height-mHeight;

			this->Width = this->Width + deltax;
			this->Height = this->Height + deltay;
			this->Refresh();

			mWidth = width;
			mHeight = height;
		}

		InitialiseRenderBuffer();
	}

	DColour* RenderView::CreateBuffer(int Width, int Height)
	{
		if (mRenderBuffer) delete [] mRenderBuffer;
		mRenderBuffer = new DColour[Width*Height];
		return mRenderBuffer;
	}

	void RenderView::TransferFloatToInt()
	{
		if (mRenderBuffer==NULL)
			return;

		System::Drawing::Color colour;
		int x, y;
		float sensitivity;
		float multiplier;
		try
		{
			sensitivity = System::Convert::ToSingle(exposureSensitivity->Text);
			multiplier = System::Convert::ToSingle(clampMultiplier->Text);
		}
		catch (System::FormatException ^/*e*/)
		{
			return;
		}
		for (y=0; y<mHeight; y++)
			for (x=0; x<mWidth; x++)
			{
				int pixelIndex = x+(y*mWidth);
				if (radioClamp->Checked==true)
				{
					int red = int(mRenderBuffer[pixelIndex].mRed*multiplier);
					if (red<0) red=0;
					if (red>255) red=255;
					int green = int(mRenderBuffer[pixelIndex].mGreen*multiplier);
					if (green<0) green=0;
					if (green>255) green=255;
					int blue = int(mRenderBuffer[pixelIndex].mBlue*multiplier);
					if (blue<0) blue=0;
					if (blue>255) blue=255;
					colour = colour.FromArgb(red, green, blue);
				}
				else if (radioExposure->Checked==true)
				{
					int red = int(255.99f * (1.f - expf(-mRenderBuffer[pixelIndex].mRed*sensitivity)));
					int green = int(255.99f * (1.f - expf(-mRenderBuffer[pixelIndex].mGreen*sensitivity)));
					int blue = int(255.99f * (1.f - expf(-mRenderBuffer[pixelIndex].mBlue*sensitivity)));
					colour = colour.FromArgb(red, green, blue);
				}
				mImage->SetPixel(x, y, colour);
			}
	}
};