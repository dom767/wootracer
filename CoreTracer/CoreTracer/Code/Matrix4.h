#pragma once

class DMatrix4
{
public:
	DMatrix4() {}

	DVector3 operator*(const DVector3& rhs) const
	{
		return DVector3(mVal[0][0] * rhs.mComponent[0] + mVal[1][0] * rhs.mComponent[1]  + mVal[2][0] * rhs.mComponent[2] + mVal[3][0],
			mVal[0][1] * rhs.mComponent[0] + mVal[1][1] * rhs.mComponent[1]  + mVal[2][1] * rhs.mComponent[2] + mVal[3][1],
			mVal[0][2] * rhs.mComponent[0] + mVal[1][2] * rhs.mComponent[1]  + mVal[2][2] * rhs.mComponent[2] + mVal[3][2]);
	}
	void operator*=(const DMatrix4& rhs)
	{
		float Result[4][4];

		Result[0][0] = mVal[0][0]*rhs.mVal[0][0] + mVal[0][1]*rhs.mVal[1][0] + mVal[0][2]*rhs.mVal[2][0] + mVal[0][3]*rhs.mVal[3][0];
		Result[0][1] = mVal[0][0]*rhs.mVal[0][1] + mVal[0][1]*rhs.mVal[1][1] + mVal[0][2]*rhs.mVal[2][1] + mVal[0][3]*rhs.mVal[3][1];
		Result[0][2] = mVal[0][0]*rhs.mVal[0][2] + mVal[0][1]*rhs.mVal[1][2] + mVal[0][2]*rhs.mVal[2][2] + mVal[0][3]*rhs.mVal[3][2];
		Result[0][3] = mVal[0][0]*rhs.mVal[0][3] + mVal[0][1]*rhs.mVal[1][3] + mVal[0][2]*rhs.mVal[2][3] + mVal[0][3]*rhs.mVal[3][3];

		Result[1][0] = mVal[1][0]*rhs.mVal[0][0] + mVal[1][1]*rhs.mVal[1][0] + mVal[1][2]*rhs.mVal[2][0] + mVal[1][3]*rhs.mVal[3][0];
		Result[1][1] = mVal[1][0]*rhs.mVal[0][1] + mVal[1][1]*rhs.mVal[1][1] + mVal[1][2]*rhs.mVal[2][1] + mVal[1][3]*rhs.mVal[3][1];
		Result[1][2] = mVal[1][0]*rhs.mVal[0][2] + mVal[1][1]*rhs.mVal[1][2] + mVal[1][2]*rhs.mVal[2][2] + mVal[1][3]*rhs.mVal[3][2];
		Result[1][3] = mVal[1][0]*rhs.mVal[0][3] + mVal[1][1]*rhs.mVal[1][3] + mVal[1][2]*rhs.mVal[2][3] + mVal[1][3]*rhs.mVal[3][3];

		Result[2][0] = mVal[2][0]*rhs.mVal[0][0] + mVal[2][1]*rhs.mVal[1][0] + mVal[2][2]*rhs.mVal[2][0] + mVal[2][3]*rhs.mVal[3][0];
		Result[2][1] = mVal[2][0]*rhs.mVal[0][1] + mVal[2][1]*rhs.mVal[1][1] + mVal[2][2]*rhs.mVal[2][1] + mVal[2][3]*rhs.mVal[3][1];
		Result[2][2] = mVal[2][0]*rhs.mVal[0][2] + mVal[2][1]*rhs.mVal[1][2] + mVal[2][2]*rhs.mVal[2][2] + mVal[2][3]*rhs.mVal[3][2];
		Result[2][3] = mVal[2][0]*rhs.mVal[0][3] + mVal[2][1]*rhs.mVal[1][3] + mVal[2][2]*rhs.mVal[2][3] + mVal[2][3]*rhs.mVal[3][3];

		Result[3][0] = mVal[3][0]*rhs.mVal[0][0] + mVal[3][1]*rhs.mVal[1][0] + mVal[3][2]*rhs.mVal[2][0] + mVal[3][3]*rhs.mVal[3][0];
		Result[3][1] = mVal[3][0]*rhs.mVal[0][1] + mVal[3][1]*rhs.mVal[1][1] + mVal[3][2]*rhs.mVal[2][1] + mVal[3][3]*rhs.mVal[3][1];
		Result[3][2] = mVal[3][0]*rhs.mVal[0][2] + mVal[3][1]*rhs.mVal[1][2] + mVal[3][2]*rhs.mVal[2][2] + mVal[3][3]*rhs.mVal[3][2];
		Result[3][3] = mVal[3][0]*rhs.mVal[0][3] + mVal[3][1]*rhs.mVal[1][3] + mVal[3][2]*rhs.mVal[2][3] + mVal[3][3]*rhs.mVal[3][3];
		
		memcpy(mVal, Result, 16*sizeof(float));
	}
	void MakeIdentity()
	{
		mVal[0][0] = 1.f; mVal[0][1] = 0.f; mVal[0][2] = 0.f; mVal[0][3] = 0.f;
		mVal[1][0] = 0.f; mVal[1][1] = 1.f; mVal[1][2] = 0.f; mVal[1][3] = 0.f;
		mVal[2][0] = 0.f; mVal[2][1] = 0.f; mVal[2][2] = 1.f; mVal[2][3] = 0.f;
		mVal[3][0] = 0.f; mVal[3][1] = 0.f; mVal[3][2] = 0.f; mVal[3][3] = 1.f;
	}
	void MakeScaleMatrix(const DVector3& scale)
	{
		mVal[0][0] = scale[0]; mVal[0][1] = 0.f; mVal[0][2] = 0.f; mVal[0][3] = 0.f;
		mVal[1][0] = 0.f; mVal[1][1] = scale[1]; mVal[1][2] = 0.f; mVal[1][3] = 0.f;
		mVal[2][0] = 0.f; mVal[2][1] = 0.f; mVal[2][2] = scale[2]; mVal[2][3] = 0.f;
		mVal[3][0] = 0.f; mVal[3][1] = 0.f; mVal[3][2] = 0.f; mVal[3][3] = 1.f;
	}
	void MakeFromRPY(float pitch, float roll, float yaw)
	{
        float cp = cosf(pitch);
        float sp = sinf(pitch);
        float sr = sinf(roll);
        float cr = cosf(roll);
        float sy = sinf(yaw);
        float cy = cosf(yaw);

        mVal[0][0] = cp * cy;
        mVal[0][1] = (sr * sp * cy) - (cr * sy);
        mVal[0][2] = (cr * sp * cy) + (sr * sy);
		mVal[0][3] = 0.0f;
        mVal[1][0] = cp * sy;
        mVal[1][1] = (sr * sp * sy) + (cr * cy);
        mVal[1][2] = (cr * sp * sy) - (sr * cy);
		mVal[1][3] = 0.0f;
        mVal[2][0] = -sp;
        mVal[2][1] = sr * cp;
        mVal[2][2] = cr * cp;	
		mVal[2][3] = 0.0f;
		mVal[3][0] = 0.0f;
		mVal[3][1] = 0.0f;
		mVal[3][2] = 0.0f;
		mVal[3][3] = 1.0f;
	}
	void MakeFromRightUpTo(const DVector3& right, const DVector3& up, const DVector3& to)
	{
		mVal[0][0] = right[0];
		mVal[0][1] = right[1];
		mVal[0][2] = right[2];
		mVal[0][3] = 0.0f;
		mVal[1][0] = up[0];
		mVal[1][1] = up[1];
		mVal[1][2] = up[2];
		mVal[1][3] = 0.0f;
		mVal[2][0] = to[0];
		mVal[2][1] = to[1];
		mVal[2][2] = to[2];
		mVal[2][3] = 0.0f;
		mVal[3][0] = 0.0f;
		mVal[3][1] = 0.0f;
		mVal[3][2] = 0.0f;
		mVal[3][3] = 1.0f;
	}
	void Invert()
	{
		//
		// Inversion by Cramer's rule.  Code taken from an Intel publication
		//
		float Result[4][4];
		float tmp[12]; /* temp array for pairs */
		float src[16]; /* array of transpose source matrix */
		float det; /* determinant */
		/* transpose matrix */
		for (int i = 0; i < 4; i++)
		{
			src[i + 0 ] = mVal[i][0];
			src[i + 4 ] = mVal[i][1];
			src[i + 8 ] = mVal[i][2];
			src[i + 12] = mVal[i][3];
		}
		/* calculate pairs for first 8 elements (cofactors) */
		tmp[0] = src[10] * src[15];
		tmp[1] = src[11] * src[14];
		tmp[2] = src[9] * src[15];
		tmp[3] = src[11] * src[13];
		tmp[4] = src[9] * src[14];
		tmp[5] = src[10] * src[13];
		tmp[6] = src[8] * src[15];
		tmp[7] = src[11] * src[12];
		tmp[8] = src[8] * src[14];
		tmp[9] = src[10] * src[12];
		tmp[10] = src[8] * src[13];
		tmp[11] = src[9] * src[12];
		/* calculate first 8 elements (cofactors) */
		Result[0][0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
		Result[0][0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
		Result[0][1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
		Result[0][1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
		Result[0][2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
		Result[0][2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
		Result[0][3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
		Result[0][3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
		Result[1][0] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
		Result[1][0] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
		Result[1][1] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
		Result[1][1] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
		Result[1][2] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
		Result[1][2] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
		Result[1][3] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
		Result[1][3] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];
		/* calculate pairs for second 8 elements (cofactors) */
		tmp[0] = src[2]*src[7];
		tmp[1] = src[3]*src[6];
		tmp[2] = src[1]*src[7];
		tmp[3] = src[3]*src[5];
		tmp[4] = src[1]*src[6];
		tmp[5] = src[2]*src[5];

		tmp[6] = src[0]*src[7];
		tmp[7] = src[3]*src[4];
		tmp[8] = src[0]*src[6];
		tmp[9] = src[2]*src[4];
		tmp[10] = src[0]*src[5];
		tmp[11] = src[1]*src[4];
		/* calculate second 8 elements (cofactors) */
		Result[2][0] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
		Result[2][0] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
		Result[2][1] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
		Result[2][1] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
		Result[2][2] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
		Result[2][2] -= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
		Result[2][3] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
		Result[2][3] -= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
		Result[3][0] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
		Result[3][0] -= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
		Result[3][1] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
		Result[3][1] -= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
		Result[3][2] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
		Result[3][2] -= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
		Result[3][3] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
		Result[3][3] -= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];
		/* calculate determinant */
		det=src[0]*Result[0][0]+src[1]*Result[0][1]+src[2]*Result[0][2]+src[3]*Result[0][3];
		/* calculate matrix inverse */
		det = 1.0f / det;

		for (UINT i = 0; i < 4; i++)
		{
			for (UINT j = 0; j < 4; j++)
			{
				mVal[i][j] = Result[i][j] * det;
			}
		}
	}
	
	float mVal[4][4];
};
