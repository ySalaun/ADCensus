#include "stereoDense/parameters.h"

/// Compute the color difference of 2 pixels
int colorDiff(float* p1, float* p2, const int wh){
	int d, diff = 0;
	for(int color = 0; color < 3; ++color){
		d = abs(int(*(p1+color*wh)-*(p2+color*wh)));
		diff = (diff > d)? diff:d;
	}
	return diff;
}

/// Compute the border of a patch centered in (x,y) in image im
/// and in the direction (dx,dy)
int PARAMETERS::patchBorder(const int x, const int y, const int dx, const int dy,
                            LWImage<float>& im)
{
	// reference pixel
	float* p = im.pixel(x, y);
	
	// coordinate of p1 the border patch pixel candidate
	int d = 1;
	int i1 = x + d*dx;
	int j1 = y + d*dy;
	
	// pixel value of p1 predecessor
	float* p2 = im.pixel(x, y);
	
	// test if p1 is still inside the picture
	while(0<=i1 && i1<w && 0<=j1 && j1<h) {
		float* p1 = im.pixel(i1, j1);
		
		// Do p1, p2 and p have similar color intensities?
		bool condition1 = colorDiff(p, p1, wh) < tau1 && colorDiff(p1, p2, wh) < tau1;
		
		// Is window limit not reached?
		bool condition2 = d < l1;

		// Better color similarities for farther neighbors?
		bool condition3 = d <= l2 || (d > l2 && colorDiff(p, p1, wh) < tau2);

		// if conditions are checked, go further in (dx, dy) direction
		if(! (condition1 && condition2 && condition3))
            break;
        ++d;
		p2 = im.pixel(i1, j1);
        i1 += dx;
        j1 += dy;
	}

	return d-1;
}

/// Compute the border of every patch in image im and in the direction (dx,dy)
/// return the value of theses borders
int* PARAMETERS::patchesBorder(const int dx, const int dy)
{
	int* borders = new int[2*w*h];
	// for each pixel in p, compute its border in (dx, dy) direction
	for(int i=0; i<=1; ++i){
		for(int x=0; x<w; ++x){
			for(int y=0; y<h; ++y){
				borders[i*w*h+x*h+y] = patchBorder(x, y, dx, dy, im[i]);
			}
		}
	}
	return borders;
}
