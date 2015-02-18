#pragma once

#include "SceneObject.h"
#include "Colour.h"

class DRay;
class TiXmlElement;

class DGlobalLighting : public DSceneObject
{
//	DECLARE_SCENE_OBJECT()

	enum EMode
	{
		PlainColour,
		Sky
	};

public:
	DGlobalLighting() : mMode(PlainColour) {}

	void Read(TiXmlElement* element);
	void SetAmbientColour(const DColour &colour) {mAmbientColour = colour;}
	const DColour& GetAmbientColour() const {return mAmbientColour;}
	void SetBackgroundColour(const DColour &colour) {mBackgroundColour = colour;}
	const DColour& GetBackgroundColour() const {return mBackgroundColour;}
	void GetSceneryColour(const DRay &Ray, DColour &Colour) const;

private:
	DColour mAmbientColour;
	DColour mBackgroundColour;
	EMode mMode;

};