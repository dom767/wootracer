#include "stdafx.h"
#include "FFT.h"
#include "Vector2.h"

/*-------------------------------------------------------------------------
	This computes an in-place complex-to-complex FFT
	x and y are the real and imaginary arrays of 2^m points.
	dir =  1 gives forward transform
	dir = -1 gives reverse transform

		Formula: forward
					N-1
					---
				1   \          - j k 2 pi n / N
		X(n) = ---   >   x(k) e                    = forward transform
				N   /                                n=0..N-1
					---
					k=0

		Formula: reverse
					N-1
					---
					\          j k 2 pi n / N
		X(n) =       >   x(k) e                    = forward transform
					/                                n=0..N-1
					---
					k=0
*/
bool FFT(int dir,int m,double *x,double *y)
{
	long nn,i,i1,j,k,i2,l,l1,l2;
	double c1,c2,tx,ty,t1,t2,u1,u2,z;

	/* Calculate the number of points */
	nn = 1;
	for (i=0;i<m;i++)
		nn *= 2;

	/* Do the bit reversal */
	i2 = nn >> 1;
	j = 0;
	for (i=0;i<nn-1;i++) {
		if (i < j) {
			tx = x[i];
			ty = y[i];
			x[i] = x[j];
			y[i] = y[j];
			x[j] = tx;
			y[j] = ty;
		}
		k = i2;
		while (k <= j) {
			j -= k;
			k >>= 1;
		}
		j += k;
	}

	/* Compute the FFT */
	c1 = -1.0;
	c2 = 0.0;
	l2 = 1;
	for (l=0;l<m;l++) {
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0;
		u2 = 0.0;
		for (j=0;j<l1;j++) {
			for (i=j;i<nn;i+=l2) {
			i1 = i + l1;
			t1 = u1 * x[i1] - u2 * y[i1];
			t2 = u1 * y[i1] + u2 * x[i1];
			x[i1] = x[i] - t1;
			y[i1] = y[i] - t2;
			x[i] += t1;
			y[i] += t2;
			}
			z =  u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}
		c2 = sqrt((1.0 - c1) / 2.0);
		if (dir == 1)
			c2 = -c2;
		c1 = sqrt((1.0 + c1) / 2.0);
	}

	/* Scaling for forward transform */
	if (dir == 1) {
		for (i=0;i<nn;i++) {
			x[i] /= (double)nn;
			y[i] /= (double)nn;
		}
	}

	return true;
}

/*-------------------------------------------------------------------------
	Calculate the closest but lower power of two of a number
	twopm = 2**m <= n
	Return TRUE if 2**m == n
*/
bool Powerof2(int n,int *m,int *twopm)
{
	if (n <= 1) {
		*m = 0;
		*twopm = 1;
		return false;
	}

	*m = 1;
	*twopm = 2;
	do {
		(*m)++;
		(*twopm) *= 2;
	} while (2*(*twopm) <= n);

	if (*twopm != n)
		return false;
	else
		return true;
}

bool DFFT::FFT2D(DVector2 *c,int nx,int ny,int dir)
{
	int i,j;
	int m,twopm;
	double *real,*imag;

	/* Transform the rows */
	real = new double [nx];
	imag = new double [nx];
	if (!real || !imag)
		return false;
	if (!Powerof2(nx,&m,&twopm) || twopm != nx)
		return false;
	for (j=0;j<ny;j++) {
		for (i=0;i<nx;i++) {
			real[i] = c[i+j*nx].x;
			imag[i] = c[i+j*nx].y;
		}
		FFT(dir,m,real,imag);
		for (i=0;i<nx;i++) {
			c[i+j*nx].x = float(real[i]);
			c[i+j*nx].y = float(imag[i]);
		}
	}
	delete [] real;
	delete [] imag;

	/* Transform the columns */
	real = new double [ny];
	imag = new double [ny];
	if (!real || !imag)
		return false;
	if (!Powerof2(ny,&m,&twopm) || twopm != ny)
		return false;
	for (i=0;i<nx;i++) {
		for (j=0;j<ny;j++) {
			real[j] = c[i+j*nx].x;
			imag[j] = c[i+j*nx].y;
		}
		FFT(dir,m,real,imag);
		for (j=0;j<ny;j++) {
			c[i+j*nx].x = float(real[j]);
			c[i+j*nx].y = float(imag[j]);
		}
	}
	delete [] real;
	delete [] imag;

	return true;
}