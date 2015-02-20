#pragma once

class TiXmlElement;
class DCamera;
class DScene;
class DColour;

class DViewport
{
public:
	DViewport(DCamera& camera, DScene& scene) : mCamera(camera), mScene(scene), mBuffer(NULL) {}
	~DViewport() {if (mBuffer) delete [] mBuffer;}
	/// Render forever
	void Render(bool singleFrame);
	/// Render me a patch please
	//void RenderPatch(int x, int y);
	/// get scene depth at specific pixel
	float GetDepth(int width, int height);

	/// Set internal pointers
	void SetBuffer(float* buffer);
	/// Copy internal buffer to the external buffer
	void CopyBuffer(DColour* buffer);

	void Read(TiXmlElement* element);
	int GetWidth() {return mWidth;}
	int GetHeight() {return mHeight;}

private:
	int mWidth;
	int mHeight;
	float* mBuffer;
	DCamera& mCamera;
	DScene& mScene;

};