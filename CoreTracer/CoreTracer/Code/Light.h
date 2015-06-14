#pragma once

#include "Vector3.h"
#include "Colour.h"
#include "SceneObject.h"

class TiXmlElement;
struct DRayContext;

class DLight
{
public:
	static DLight* CreateFromXML(TiXmlElement* element);
	virtual void Read(TiXmlElement* element)=0;
	virtual bool GetLighting(const DScene& scene, const DRayContext &rayContext, const DVector3& reflection, const float specularPower, const DColour& specularColour, DColour& out_Diffuse, DColour& out_Specular)=0;
};

class DPointLight : public DLight
{
public:
	DPointLight();
	virtual void Read(TiXmlElement* element);
	virtual bool GetLighting(const DScene& scene, const DRayContext &rayContext, const DVector3& reflection, const float specularPower, const DColour& specularColour, DColour& out_Diffuse, DColour& out_Specular);

	DVector3 mPosition;
	DColour mColour;
};

class DWorldLight : public DLight
{
public:
	DWorldLight() {}
	virtual void Read(TiXmlElement* element);
	virtual bool GetLighting(const DScene& scene, const DRayContext &rayContext, const DVector3& reflection, const float specularPower, const DColour& specularColour, DColour& out_Diffuse, DColour& out_Specular);

	DColour mColour;
	int mSamples;
};

class DAmbientLight: public DLight
{
public:
	DAmbientLight() {}
	virtual void Read(TiXmlElement* element);
	virtual bool GetLighting(const DScene& scene, const DRayContext &rayContext, const DVector3& reflection, const float specularPower, const DColour& specularColour, DColour& out_Diffuse, DColour& out_Specular);

	DColour mColour;
};

class DDirectionalLight : public DLight
{
public:
	DDirectionalLight() : mArea(0) {}
	virtual void Read(TiXmlElement* element);
	virtual bool GetLighting(const DScene& scene, const DRayContext &rayContext, const DVector3& reflection, const float specularPower, const DColour& specularColour, DColour& out_Diffuse, DColour& out_Specular);

	DVector3 mDirection;
	DColour mColour;
	float mArea;
};

class DSphereLight : public DLight
{
public:
	DSphereLight() {}
	virtual void Read(TiXmlElement* element);
	virtual bool GetLighting(const DScene& scene, const DRayContext &rayContext, const DVector3& reflection, const float specularPower, const DColour& specularColour, DColour& out_Diffuse, DColour& out_Specular);

	DVector3 mPosition;
	float mRadius;
	int mSamples;
	DColour mColour;
};