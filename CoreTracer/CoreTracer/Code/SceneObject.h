#pragma once

#include "Colour.h"
#include "Vector3.h"
#include <string>
#include <sstream>
#include <vector>
#include "SceneIO.h"

/// All scene objects must derive off this class
/// Note that this class handles XML load/save. This should really be moved
/// somewhere else, but for now it works.
class DSceneObject
{
public:
	/// user overrides
	virtual void PostLoad() {}
	/// Set the object ID (used by creation code)
	void SetObjectId(const int ObjectId)
	{
		mObjectId = ObjectId;
	}
	//@{ Accessors
	const int GetObjectId() const {return mObjectId;}
	//@}

protected:

	//@{ Common data members for all scene objects
	int mObjectId;
	//@}

};
