#pragma once

#include "Colour.h"
#include "ColourFunction.h"
#include "FuncCore.h"

class DVector3;
struct DRayContext;
class TiXmlElement;
class DVecFunc;

class DMaterial
{
public:
	void Read(TiXmlElement* element);
	DCF* CreateDCF(TiXmlElement* DCFXml);

	DMaterial() : mDiffuseColour(new DCFConstant(1.f, 1.f, 1.f)),
		mSpecularColour(new DCFConstant(0.f, 0.f, 0.f)),
		mEmissiveColour(new DCFConstant(0.f, 0.f, 0.f)),
		mSpecularPower(20),
		mReflectivity(new DCFConstant(0.5, 0.5, 0.5)),
		m_Opacity(1.f),
		m_RefractiveIndex(1.f),
		m_Density(1.f),
		m_Shininess(1.f),
		mAbsorptionColour(new DCFConstant(0.f, 0.f, 0.f)) {}
	DMaterial(const DCFConstant& colour, const DColour& specularColour, const float specularPower, const float reflectivity, const float Opacity, const float RefractiveIndex, const float Density, const DColour& Absorbtion)
		: mDiffuseColour(new DCFConstant(colour)),
		mSpecularColour(new DCFConstant(specularColour)),
		mEmissiveColour(new DCFConstant(0.f, 0.f, 0.f)),
		mSpecularPower(specularPower),
		mReflectivity(new DCFConstant(reflectivity, reflectivity, reflectivity)),
		m_Opacity(Opacity),
		m_RefractiveIndex(RefractiveIndex),
		m_Density(Density),
		m_Shininess(1.f),
		mAbsorptionColour(new DCFConstant(Absorbtion)) {}
	
	~DMaterial()
	{
		Cleanup();
	}

	/// accessors
	void SetSpecularPower(const float specularPower) {mSpecularPower=specularPower;}
	void SetOpacity(const float Opacity) {m_Opacity = Opacity;}
	void SetRefractiveIndex(const float RefractiveIndex) {m_RefractiveIndex = RefractiveIndex;}
	void SetDensity(const float Density) {m_Density = Density;}
	void SetShininess(const float shininess) {m_Shininess = shininess;}

	void Cleanup();

	/// meat and 2 veg
	void CalculateColour(DColour &out_colour,
		const DVector3& hitPos,
		const int hitId,
		const DVector3& normal,
		const DRayContext& rRayContext,
		const bool ignoreLighting) const;

private:
	DProgram mMaterialProgram;
	DCF* mDiffuseColour;
	DCF* mSpecularColour;
	DCF* mEmissiveColour;
	float mSpecularPower;
	DCF* mReflectivity;
	float m_Opacity;
	float m_RefractiveIndex;
	float m_Density;
	float m_Shininess;
	DCF* mAbsorptionColour;
};