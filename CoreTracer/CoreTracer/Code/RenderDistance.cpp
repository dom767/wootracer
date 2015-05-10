#include "stdafx.h"
#include "RenderDistance.h"
#include "Scene.h"
#include "tinyxml.h"
#include "convert.h"
#include "log.h"
#include "Functions.h"



void DRenderDistance::InternalRead(TiXmlElement* element)
{
	const char* distanceEvaluator = element->Attribute("distancefunction");
//	std::string de = "mandelbox(pos,15,3.24)";//"sphere(pos, vec(0,0,0), 1)";
//	std::string de = "smin(sphere(pos, vec(-0.5, 0, 0), 0.5), box(pos, vec(0.5, 0.0, 0.0), vec(0.5,0.5,0.5)), 10)";
//	std::string de = "torus(rotx(pos,90), 0.6, 0.3)";
//	std::string de = "smin(torus(rotx(pos,90), 0.6, 0.15), torus(pos, 0.6, 0.15), 10)";
//	std::string de = "smin3(torus(roty(pos,90), 0.6, 0.15), torus(rotx(pos,90), 0.6, 0.15), torus(pos, 0.6, 0.15), 10)";
	std::string de = "set(distance, sphere(cosx(pos, 1.0), vec(0, 0, 0), 1.0))";
//	std::string de = "torus(roty(repxyz(pos, vec(0.1, 0.31, 0.1)), mul(getx(pos),70)), 0.04, 0.01)";
//	std::string de = "torus(roty(pos, 60), 0.8, 0.2)";
//	std::string de = "smin(sphere(pos, vec(0,0.5,0), 0.5), sphere(pos, vec(0,-0.5,0), 0.5), 20)";
//	std::string de = "kaleido(pos, vec(0.4,0,0), vec(0,0,0), vec(1,1,1))";
	if (distanceEvaluator) de = distanceEvaluator;

	Convert::StrToFloat(element->Attribute("minimumdistance"), mMinimumDistance);
	Convert::StrToFloat(element->Attribute("distancescale"), mDEScale);
	Convert::StrToVec3(element->Attribute("distanceoffset"), mDEOffset);
	Convert::StrToInt(element->Attribute("distanceiterations"), mDEIterations);
	Convert::StrToVec3(element->Attribute("distanceextents"), mDEExtents);
	Convert::StrToFloat(element->Attribute("stepsize"), mStepSize);

// two spheres	std::string de = "min(sphere(vec(-0.5,0,0),0.5), sphere(vec(0.5,0,0),0.5))";
// two spheres smooth	std::string de = "smin(sphere(vec(-0.5,0,0),0.5), sphere(vec(0.5,0,0),0.5), 10)";
// lens	std::string de = "max(sphere(pos, vec(0, 0, 0.5),0.5), neg(sphere(pos,vec(0,0,-0.1),0.8)))";
// sphere from first principles	std::string de = "sub(mag(subv(pos,vec(0,0,0))),1.0)";
	mDistanceProgram.Create(de);
	
	PostLoad();
}

float sphere(DVector3 pos, DVector3 centre, float radius)
{
	return (pos-centre).Magnitude() - radius;
}

float sum(float a, float b)
{
	return a+b;
}
/*
float min(float a, float b)
{
	if (a<b)
		return a;
	else
		return b;
}*/

float smin( float a, float b, float k )
{
    float res = exp( -k*a ) + exp( -k*b );
    return -log( res )/k;
}

float tetra(DVector3 p)
{
/*	vec3 a1 = vec3(1,1,1);
	vec3 a2 = vec3(-1,-1,1);
	vec3 a3 = vec3(1,-1,-1);
	vec3 a4 = vec3(-1,1,-1);
	vec3 c;
	int n = 0;
	float dist, d;
	while (n < Iterations) {
		 c = a1; dist = length(z-a1);
	        d = length(z-a2); if (d < dist) { c = a2; dist=d; }
		 d = length(z-a3); if (d < dist) { c = a3; dist=d; }
		 d = length(z-a4); if (d < dist) { c = a4; dist=d; }
		z = Scale*z-c*(Scale-1.0);
		n++;
	}

	return length(z) * pow(Scale, float(-n));
*/
	DVector3 a1 = DVector3(1,1,1);
	DVector3 a2 = DVector3(-1,1,-1);
	DVector3 a3 = DVector3(1,-1,-1);
	DVector3 a4 = DVector3(-1,-1,1);
	DVector3 c;
	int n = 0;
	float dist, d;
	float Scale = 2.f;
	while (n < 2/*Iterations*/) {
		 c = a1; dist = (p-a1).Magnitude();
	        d = (p-a2).Magnitude(); if (d < dist) { c = a2; dist=d; }
		 d = (p-a3).Magnitude(); if (d < dist) { c = a3; dist=d; }
		 d = (p-a4).Magnitude(); if (d < dist) { c = a4; dist=d; }
		p = (p*Scale)-c*(Scale-1.0f);
		n++;
	}

	return p.Magnitude() * pow(Scale, float(-n));
}

float tetra2(DVector3 p)
{
//    float r;
    int n = 0;
	float Scale = 2;
	DVector3 Offset = DVector3(0,0,0);
    while (n < 3/*Iterations*/) {
		if(p.mComponent[0]+p.mComponent[2]<0) {p.mComponent[0]=-p.mComponent[2];p.mComponent[2]=-p.mComponent[0];} //p.xy = -p.yx; // fold 1
       if(p.mComponent[0]+p.mComponent[1]<0) {p.mComponent[0]=-p.mComponent[1];p.mComponent[1]=-p.mComponent[0];} //p.xz = -p.zx; // fold 2
       if(p.mComponent[2]+p.mComponent[1]<0) {p.mComponent[1]=-p.mComponent[2];p.mComponent[2]=-p.mComponent[1];} //p.zy = -p.yz; // fold 3	
       p = p*Scale - Offset*(Scale-1.0f);
       n++;
    }
    return (p.Magnitude() ) * pow(Scale, -float(n));
}

float manyspheres(DVector3 pos)
{
	pos.mComponent[0] = abs(fmodf(pos.mComponent[0], 0.1f))-0.05f;
	pos.mComponent[2] = abs(fmodf(pos.mComponent[2], 0.1f))-0.05f;
	return pos.Magnitude() - 0.03f;
}

float mandelbulb(DVector3 pos)
{
/*  float x = pos.mComponent[0]; float x2 = x*x; float x4 = x2*x2;
  float y = pos.mComponent[1]; float y2 = y*y; float y4 = y2*y2;
  float z = pos.mComponent[2]; float z2 = z*z; float z4 = z2*z2;

  float k3 = x2 + z2;
  float k2 = 1 / sqrtf( k3*k3*k3*k3*k3*k3*k3 );
  float k1 = x4 + y4 + z4 - 6.0f*y2*z2 - 6.0f*x2*y2 + 2.0f*z2*x2;
  float k4 = x2 - y2 + z2;

  DVector3 ret;
  ret.mComponent[0] =  64.0f*x*y*z*(x2-z2)*k4*(x4-6.0f*x2*z2+z4)*k1*k2;
  ret.mComponent[1] = -16.0f*y2*k3*k4*k4 + k1*k1;
  ret.mComponent[2] = -8.0f*y*k4*(x4*x4 - 28.0f*x4*x2*z2 + 70.0f*x4*z4 - 28.0f*x2*z2*z4 + z4*z4)*k1*k2;
  return ret.Magnitude();
*/
	float mIterations = 10;
	float mBailout = 4;
	float mPower = 8;
	DVector3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	for (int i = 0; i < mIterations ; i++) {
		r = z.Magnitude();
		if (r>mBailout) break;
		
		// convert to polar coordinates
		float theta = acosf(z[2]/r);
		float phi = atanf(z[1]/z[0]);
		dr =  powf( r, mPower-1.0f)*mPower*dr + 1.0f;
		
		// scale and rotate the point
		float zr = pow( r,mPower);
		theta = theta*mPower;
		phi = phi*mPower;
		
		// convert back to cartesian coordinates
		z = DVector3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta))*zr;
		z+=pos;
	}
	return 0.5f*log(r)*r/dr;
}

float mandelbox(DVector3 pos)
{
	int iterations = 10;
	float scale = 4;

	            DVector3 c = pos;
            float dr = 1.0f;
            for (int n = 0; n < iterations; n++)
            {
                if (pos[0] > 1)
                    pos[0] = 2 - pos[0];
                else if (pos[0] < -1)
                    pos[0] = -2 - pos[0];
                if (pos[1] > 1)
                    pos[1] = 2 - pos[1];
                else if (pos[1] < -1)
                    pos[1] = -2 - pos[1];
                if (pos[2] > 1)
                    pos[2] = 2 - pos[2];
                else if (pos[2] < -1)
                    pos[2] = -2 - pos[2];

                float r2 = pos.MagnitudeSquared();

                if (r2 < 1)
                {
                    if (r2 < 0.25f) // 0.5 squared
                    {
                        pos *= 4;
                        dr *= 4;
                    }
                    else
                    {
                        pos /= r2;
                        dr /= r2;
                    }
                }

                pos = pos * scale + c;
                dr = dr * abs(scale) + 1.0f;
            }
            return pos.Magnitude() / abs(dr);
}

void rotate1(DVector3& pos)
{
	DMatrix4 mat;
	mat.MakeFromRPY(0.4f,0,0);
	pos = mat * pos;
	
}

float sierpinski3(DVector3 pos){
	float scale=2;
	float bailout = 1000;
   float r=pos.MagnitudeSquared();
   int i;
   for(i=0;i<10 && r<bailout;i++){
      //Folding... These are some of the symmetry planes of the tetrahedron
      float tmp;

	  rotate1(pos);

	   if(pos[0]+pos[1]<0){tmp=-pos[1];pos[1]=-pos[0];pos[0]=tmp;}
      if(pos[0]+pos[2]<0){tmp=-pos[2];pos[2]=-pos[0];pos[0]=tmp;}
      if(pos[1]+pos[2]<0){tmp=-pos[2];pos[2]=-pos[1];pos[1]=tmp;}
      
      //Stretche about the point [1,1,1]*(scale-1)/scale; The "(scale-1)/scale" is here in order to keep the size of the fractal constant wrt scale
      pos[0]=scale*pos[0]-(scale-1);//equivalent to: x=scale*(x-cx); where cx=(scale-1)/scale;
      pos[1]=scale*pos[1]-(scale-1);
      pos[2]=scale*pos[2]-(scale-1);
      r=pos.MagnitudeSquared();
   }	
   return (sqrtf(r)-2)*powf(scale,-float(i));//the estimated distance
}

float DRenderDistance::DistanceEstimator(DVector3& pos) const
{
//	return sphere(pos, DVector3(0,0,0), 0.9f);
//	return mandelbulb(pos);
//	return sierpinski3(pos);
	DFunctionState funcState;
	funcState.mObjectPosition = pos;
	funcState.mDistance = 0.0f;
	mDistanceProgram.Run(funcState);
	return funcState.mDistance;
	return max(sphere(pos, DVector3(0, 0, 0.3f), 0.7f), - sphere(pos, DVector3(0,0,-0.5f), 0.5f));
	return smin(sphere(pos, DVector3(0.5f,0,0), 0.5f), sphere(pos, DVector3(-0.5f,0,0), 0.5f), 10);
	//return manyspheres(pos);
	//return mandelbulb(pos);
	//return tetra2(pos);//smin(sphere(pos, DVector3(0.5f,0,0), 0.5f), sphere(pos, DVector3(-0.5f,0,0), 0.5f), 10);
}

DVector3 DRenderDistance::GetNormal(const DVector3& position, const DVector3& direction) const
{
	const float normalTweak=mMinimumDistance*0.1f;
	DVector3 normal = DVector3(DistanceEstimator(position+DVector3(normalTweak,0,0)) - DistanceEstimator(position-DVector3(normalTweak,0,0)),
		DistanceEstimator(position+DVector3(0,normalTweak,0)) - DistanceEstimator(position-DVector3(0,normalTweak,0)),
		DistanceEstimator(position+DVector3(0,0,normalTweak)) - DistanceEstimator(position-DVector3(0,0,normalTweak)));

	// sigh...
	float MagSq = normal.MagnitudeSquared();
	if (MagSq == 0)
	{
		normal[0] = -direction[0];
		normal[1] = -direction[1];
		normal[2] = -direction[2];
		return normal;
	}
	else
	{
		// normalise (already got the magsq)
		normal /= sqrtf(MagSq);
		return normal;
	}
}

void DRenderDistance::Nudge(DVector3& out_Position, const DVector3& normal) const
{
	DVector3 delta = normal * (1.001f*mMinimumDistance);
	float distance;
	// push the delta location out a bit if we're still inside the object surface
	float deltaDistance = DistanceEstimator(out_Position + delta);

	if (deltaDistance < mMinimumDistance)
	{
		float startpos = 1.001f;
		float endpos = 2.f;
		for (int i=0; i<4; i++)
		{
			float mid = (endpos+startpos)*0.5f;
			delta = normal * (mid*mMinimumDistance);
			distance = DistanceEstimator(out_Position + delta);
			if (distance < mMinimumDistance)
				startpos = mid;
			else
				endpos = mid;
		}
		if (distance < mMinimumDistance)
		{
			delta = normal * (endpos*mMinimumDistance);
		}
	}				

	out_Position += delta;
}

void DRenderDistance::DeNudge(DVector3& out_Position, const DVector3& normal) const
{
	DVector3 delta = normal * (1.001f*mMinimumDistance);
	float distance;
	// push the delta location out a bit if we're still inside the object surface
	float deltaDistance = DistanceEstimator(out_Position - delta);

	if (deltaDistance > mMinimumDistance)
	{
		float startpos = 1.001f;
		float endpos = 2.f;
		for (int i=0; i<4; i++)
		{
			float mid = (endpos+startpos)*0.5f;
			delta = normal * (mid*mMinimumDistance);
			distance = DistanceEstimator(out_Position - delta);
			if (distance > mMinimumDistance)
				startpos = mid;
			else
				endpos = mid;
		}
		if (distance > mMinimumDistance)
		{
			delta = normal * (endpos*mMinimumDistance);
		}
	}				

	out_Position -= delta;
}

bool DRenderDistance::GetCollisionPoint(DVector3 &out_Point, const DVector3& start, const DVector3& end, const DVector3& direction) const
{
	// step no further than this
	float endT = (end - start).Magnitude();
	out_Point = start;
	float oldDistance = 0.0f;
	float totalDistance = 0.0f;

	for (int steps=0; steps < mDEIterations; steps++)
	{
		float distance = DistanceEstimator(out_Point);

		if (steps==0 && distance<mMinimumDistance)	
		{
			out_Point = start;
			return true;
		}

		// stepped too far...
		if (distance<0)
		{
			float starttd = oldDistance;
			float endtd = totalDistance;
			while (endtd-starttd>mMinimumDistance*0.1 && (distance<0 || distance>mMinimumDistance))
			{
				float mid = (endtd+starttd)*0.5f;
				out_Point = start + direction * mid;
				distance = DistanceEstimator(out_Point);
				if (distance < mMinimumDistance)
					endtd = mid;
				else
					starttd = mid;
			}
			if (distance<0)
			{
				out_Point = start + direction * starttd;
				distance = mMinimumDistance*0.5f;//DistanceEstimator(p);
			}
		}

		if (distance < mMinimumDistance
			&& totalDistance < endT)
		{
			return true;
		}

		oldDistance = totalDistance;
		totalDistance += distance*mStepSize;
		out_Point += direction * distance*mStepSize;

		if (totalDistance > endT)
			return false;
	}

	return false;
}

bool DRenderDistance::GetBackfaceCollisionPoint(DVector3 &out_Point, const DVector3& start, const DVector3& end, const DVector3& direction) const
{
	// step no further than this
	float endT = (end - start).Magnitude();
	out_Point = start;
	float oldDistance = 0.0f;
	float totalDistance = 0.0f;

	for (int steps=0; steps < mDEIterations; steps++)
	{
		float distance = DistanceEstimator(out_Point);

		if (steps==0 && distance>mMinimumDistance)	
		{
			out_Point = start;
			return true;
		}

		// stepped too far...
		if (distance>mMinimumDistance*2)
		{
			float starttd = oldDistance;
			float endtd = totalDistance;
			while (endtd-starttd>mMinimumDistance*0.1 && (distance<mMinimumDistance || distance>mMinimumDistance*2))
			{
				float mid = (endtd+starttd)*0.5f;
				out_Point = start + direction * mid;
				distance = DistanceEstimator(out_Point);
				if (distance > mMinimumDistance*2)
					endtd = mid;
				else
					starttd = mid;
			}
			if (distance>mMinimumDistance*2)
			{
				out_Point = start + direction * starttd;
				distance = mMinimumDistance*0.5f;//DistanceEstimator(p);
			}
		}

		if (distance > mMinimumDistance
			&& totalDistance < endT)
		{
			return true;
		}

		oldDistance = totalDistance;
		totalDistance += (2*mMinimumDistance-distance)*mStepSize;
		out_Point += direction * (2*mMinimumDistance-distance)*mStepSize;

		if (totalDistance > endT)
			return false;
	}

	return false;
}

bool DRenderDistance::InternalIntersect(const DRayContext& rRayContext, DCollisionResponse& out_Response) const
{
	// parse failed
	if (!mDistanceProgram.Compiled())
		return false;

	// cache direction vector
	const DVector3& direction = rRayContext.m_Ray.GetDirection();
	
	// calculate in/out positions on the DE bounding box
	DVector3 AAstart, AAend;
	DAABoundingBox aaBoundingBox(DVector3(0,0,0),mDEExtents[0],mDEExtents[1],mDEExtents[2]);
	if (!aaBoundingBox.Intersects(rRayContext.m_Ray, AAstart, AAend))
		return false; // Looks like we're not hittng the DE object, weird!!
	const DVector3 start = (AAstart - mDEOffset) / mDEScale;
	const DVector3 end = (AAend - mDEOffset) / mDEScale;

	DVector3 p;
	DVector3 normal;

	if ((rRayContext.m_RequestFlags&RequestBackface)==0)
	{
		// work out how far away our hit is...
		if (!GetCollisionPoint(p, start, end, direction))
			return false;

		normal = GetNormal(p, direction);
		Nudge(p, normal);
	}
	else
	{
		// BACKFACE FTW
		DVector3 newStart = start;
		normal = GetNormal(start, direction);
		DeNudge(newStart, normal);

		// work out how far away our hit is...
		if (!GetBackfaceCollisionPoint(p, newStart, end, direction))
			return false;

		normal = GetNormal(p, direction);
		Nudge(p, normal);
		normal.Negate();
	}
	
	out_Response.mNormal = normal;
	out_Response.mObjectPosition = p;
	out_Response.mObjectPosition *= mDEScale;
	out_Response.mObjectPosition += mDEOffset;
	return true;
}