#include "stereoDense/parameters.h"

/// patchBorder computes the border of a patch centered in (x,y) in image im and in the direction (dx,dy)
int PARAMETERS::patchBorder(const int x, const int y, const int dx, const int dy, const int index)
{
	const LWImage<float> im = (index == 1)? im1: im2;
	const float p = *(im.pixel(x, y));
	
	// coordinate of p1 the border patch pixel candidate
	int d = 1;
	int i1 = x + d*dx;
	int j1 = y + d*dy;
	
	// pixel value of p1 predecessor
	float p2 = p;
	
	// test if p1 is still inside the picture
	while(i1 >= 0 && i1 < w && j1 >= 0 && j1 < h){
		float p1 = *(im.pixel(i1, j1));
		
		// test if p1, p2 and p have similar color intensities
		bool condition1 = abs(int(p1 - p)) < tau1 && abs(int(p1 - p2)) < tau1;
		
		// test if the window limit is not reached
		bool condition2 = d < l1;

		// test for better color similarities for farther neighbours
		bool condition3 = d <= l2 || (d > l2 && abs(int(p1 - p)) < tau2);

		// if conditions are checked, go further in (dx, dy) direction
		if(condition1 && condition2 && condition3){
			++ d;
			i1 = x + d*dx;
			j1 = y + d*dy;
			p2 = p1;
		}
		else{
			break;
		}
	}

	return d-1;
}

/// patchesBorder computes the border of every patches in image im and in the direction (dx,dy)
/// return the value of theses borders
int* PARAMETERS::patchesBorder(const int dx, const int dy)
{
	int x, y, index;
	int* borders = new int[2 * w * h];
	// for each pixel in p, compute its border in (dx, dy) direction
	for(index = 1; index < 3; ++index){
		for(x = 0; x < w; ++x){
			for(y = 0; y < h; ++y){
				borders[(index-1)*w*h+x*h+y] = patchBorder(x, y, dx, dy, index);
			}
		}
	}
	return borders;
}
