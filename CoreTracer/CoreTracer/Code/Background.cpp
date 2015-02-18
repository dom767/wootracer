#include "stdafx.h"
#include "Background.h"
#include "SceneIO.h"
#include "Ray.h"
#include "RenderSphere.h"
#include "Scene.h"
#include "convert.h"
#include "tinyxml.h"

void DBackground::GetSceneryColour(const DRay& Ray, DColour& Colour) const
{
	switch(mMode)
	{
	case PlainColour:
		Colour = mBackgroundColour;
		return;
	case Sky:
		{
/*			// These rays are in km for precision reasons
			DRay RootAtmosphereRay(DVector3(0, 6000.f, 0), Ray.GetDirection());
			DRenderSphere Sphere(DVector3(0,0,0), 6030.f, DMaterial());
			DVector3 SunPosition(1, 1, 0);
			SunPosition.Normalise();
			DColour RayleighScattering(0.6f, 0.8f, 0.98f);

			// calculate distance ray moves through atmosphere
			DCollisionResponse Response;
			Sphere.Intersect(DRayContext(NULL, RootAtmosphereRay, RequestDistance|RequestBackface, 0, DRayContext::AirRefractionIndex), Response);
			const float ScatteringScaling = 0.01f;
			float AtmosphereDistance = ScatteringScaling*Response.mDistance;

			// calculate directional effect (cos(theta)^2*C)
			// http://hyperphysics.phy-astr.gsu.edu/Hbase/atmos/blusky.html
			float DirectionElement = SunPosition.Dot(Ray.GetDirection());
			DirectionElement = (0.5f * DirectionElement * DirectionElement) + 0.5f;

			// calculate rayleigh scattering colour, note that light accumulates
			// linearly with distance, which seems counterintuitive but seems to work fine
			Colour = DColour((RayleighScattering.mRed * AtmosphereDistance * DirectionElement),
				(RayleighScattering.mGreen * AtmosphereDistance * DirectionElement),
				(RayleighScattering.mBlue * AtmosphereDistance * DirectionElement));

			DColour MieScattering;
			float CosDirection = SunPosition.Dot(Ray.GetDirection()); // sun component
			if (CosDirection<0.f) CosDirection = 0.f;
			float MieCoefficient = 2.f * pow(CosDirection, 4);
			CosDirection = CosDirection * 0.7f + 0.3f;
			MieScattering = DColour(MieCoefficient, MieCoefficient, MieCoefficient);

			Colour += MieScattering;
*/			break;
		}
	default:
		Colour = DColour(1.f, 0.f, 0.f);
		return;
	}
}

void DBackground::Read(TiXmlElement* element)
{
//	Convert::StrToVec3(element->Attribute("position"), mPosition);
	int intMode;
	Convert::StrToInt(element->Attribute("type"), intMode);
	mMode = (intMode == 0) ? PlainColour : Sky;
	Convert::StrToCol(element->Attribute("backgroundColour"), mBackgroundColour);
}