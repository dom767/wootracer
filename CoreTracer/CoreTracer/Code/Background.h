#pragma once

#include "SceneObject.h"
#include "Colour.h"

class DRay;
class TiXmlElement;

class DBackground : public DSceneObject
{
	enum EMode
	{
		PlainColour,
		Sky
	};

public:
	DBackground() : mMode(PlainColour) {}

	void Read(TiXmlElement* element);
	void SetBackgroundColour(const DColour &colour) {mBackgroundColour = colour;}
	const DColour& GetBackgroundColour() const {return mBackgroundColour;}
	void GetSceneryColour(const DRay &Ray, DColour &Colour) const;

private:
	DColour mBackgroundColour;
	EMode mMode;

};