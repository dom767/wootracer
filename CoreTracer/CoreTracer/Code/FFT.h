#pragma once

class DVector2;

// Borrowed from Paul Bourke, ta matey
// http://paulbourke.net/miscellaneous/dft/

class DFFT
{
public:
	/*-------------------------------------------------------------------------
	   Perform a 2D FFT inplace given a complex 2D array
	   The direction dir, 1 for forward, -1 for reverse
	   The size of the array (nx,ny)
	   Return false if there are memory problems or
		  the dimensions are not powers of 2
	*/
	static bool FFT2D(DVector2 *c,int nx,int ny,int dir);
};
