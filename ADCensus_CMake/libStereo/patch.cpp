/* Distance between patches.
    Copyright (C) 2010-2012 Pascal Monasse <monasse@imagine.enpc.fr> & Yohann Salaun <yohann.salaun@polytechnique.org

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "patch.h"
#include "math.h"

/// Sum of pixel values in patch centered on (i,j).
float sum(const LWImage<float>& im, int i, int j, int win)
{
    float s=0.0f;
	for(int y=-win; y<=win; y++) {
		const float* p = im.pixel(i-win, j+y);
        for(int x=-win; x<=win; x++)
            s += *p++;
    }
    return s;
}

/// Sum of Square Differences between patches centered on (i1,j1) and (i2,j2).
float ssd(const LWImage<float>& im1, int i1,int j1, 
          const LWImage<float>& im2, int i2,int j2, int win)
{
	float dist=0.0f;
	for(int j=-win; j<=win; j++) {
		const float* p1 = im1.pixel(i1-win, j1+j);
		const float* p2 = im2.pixel(i2-win, j2+j);
		for(int i=-win; i<=win; i++){
			float dif = (*p1++ - *p2++);
			dist += (dif*dif);
		}
	}
	return dist;
}

/// Centered Sum of Square Differences of patches of size 2*win+1.
float cssd(const LWImage<float>& im1,int i1,int j1,
           const LWImage<float>& im2,int i2,int j2, int win)
{
    float m = sum(im1,i1,j1,win)-sum(im2,i2,j2,win);
    int w = 2*win+1;
    return (ssd(im1,i1,j1, im2,i2,j2, win) - m*m/(w*w));
}

/// Absolute Differences between pixels (i1,j1) and (i2,j2).
float ad(const LWImage<float>& im1, int i1,int j1, 
         const LWImage<float>& im2, int i2,int j2)
{
	const float* p1 = im1.pixel(i1, j1);
	const float* p2 = im2.pixel(i2, j2);
	const float dist = (float)(std::abs((int)(*p1 - *p2)));
	return dist;
}

/// Census measure between 9x7 patches centered on (i1,j1) and (i2,j2).
/// Computes the Hamming distance between the structures around each pixels
float census(const LWImage<float>& im1, int i1,int j1, 
			 const LWImage<float>& im2, int i2,int j2, const int winI, const int winJ)
{
	const float p1ref = *(im1.pixel(i1, j1));
	const float p2ref = *(im2.pixel(i2, j2));
	float dist=0.0f;
	for(int j=-winJ; j<=winJ; j++) {
		const float* p1 = im1.pixel(i1-winI, j1+j);
		const float* p2 = im2.pixel(i2-winI, j2+j);
		for(int i=-winI; i<=winI; i++){
			bool dif = (*p1++ - p1ref)*(*p2++ - p2ref) < 0;
			if(dif){
				dist += 1.0;
			}
		}
	}
	return dist;
}
/// adCensus computes a cost defined by:
/// Cost = (1-exp(COST_AD/lambdaAD)) + (1-exp(COST_CENSUS/lambdaCensus))
/// it is a combination of the cost functions AD and Census explained above
float adCensus(const LWImage<float>& im1, int i1,int j1, 
			   const LWImage<float>& im2, int i2,int j2, const int winI, const int winJ,
			   const float lambdaAD, const float lambdaCensus)
{
	float dist = 0.0f;
	
	const float cAD = ad(im1, i1, j1, im2, i2, j2);
	const float cCensus = census(im1, i1, j1, im2, i2, j2, winI, winJ);

	dist += 1-exp(-cAD/lambdaAD);
	dist += 1-exp(-cCensus/lambdaCensus);

	return dist;
}

/// patchBorder computes the border of a patch centered in (i,j) in image im and in the direction (dx,dy)
int patchBorder(const LWImage<float>& im, const int i, const int j,
				const int di, const int dj,
				const int l1, const int l2, const float tau1, const float tau2)
{
	const float p = *(im.pixel(i, j));
	
	// coordinate of p1 the border patch pixel candidate
	int d = 1;
	int i1 = i + d*di;
	int j1 = j + d*dj;
	
	// pixel value of p1 predecessor
	float p2 = p;
	
	// test if p1 is still inside the picture
	while(i1 >= 0 && i1 < im.w && j1 >= 0 && j1 < im.h){
		float p1 = *(im.pixel(i1, j1));
		
		// test if p1, p2 and p have similar color intensities
		bool condition1 = abs(p1 - p) < tau1 && abs(p1 - p2) < tau1;
		
		// test if the window limit is not reached
		bool condition2 = d < l1;

		// test for better color similarities for farther neighbours
		bool condition3 = d > l2 && abs(p1 - p) < tau2;

		// if conditions are checked, go further in (di, dj) direction
		if(condition1 && condition2 && condition3){
			++ d;
			int i1 = i + d*di;
			int j1 = j + d*dj;
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
int* patchesBorder(const LWImage<float>& im, const int di, const int dj,
				   const int l1, const int l2, const float tau1, const float tau2)
{
	int i, j;
	int* borders = new int[im.w * im.h];
	// for each pixel in p, compute its border in (di, dj) direction
	for(i = 0; i < im.w; ++i){
		for(j = 0; j < im.h; ++j){
			borders[i*im.h+j] = patchBorder(im, i, j, di, dj, l1, l2, tau1, tau2);
		}
	}
	return borders;
}

/// agregateCosts1D computes the agregated costs of a table costs in the direction (di, dj) and with the borders given as parameters
/// d gives the current disparity
/// it returns a table of the agregated costs
float* agregateCosts1D(float* costs, int w, int h, int disparity,
					   const float* leftBorders, const float* rightBorders,
					   int di, int dj)
{
	int i, j, d, dmin, dmax;
	float agregatedCost;
	float* agregatedCosts = new float[w*h];
	
	// agregate costs in the direction (di, dj)
	for(i = 0; i < w; ++i){
		for(j = 0; j < h; ++j){
			agregatedCost = 0;
			dmin = -leftBorders[i*h+j];
			dmax = rightBorders[i*h+j];
			for(d = dmin; d<= dmax; ++d){
				agregatedCost += costs[disparity*h*w+(i+d*di)*h+(j+d*dj)];
			}
			agregatedCosts[i*h+j] = agregatedCost;
		}
	}
	return agregatedCosts;
}

/// agregateCosts2D computes the agregated costs of a table costs vertically and horizontally
/// horizontalFirst gives the order and the ~Borders table give the borders in each direction
/// d gives the current disparity
/// it replaces the costs table by its agregated correspondent
void agregateCosts2D(float* costs, int w, int h, int d,
					 const float* leftBorders, const float* rightBorders,
					 const float* upBorders, const float* downBorders,
					 bool horizontalFirst)
{
	int step, i, j;
	float *temporaryCosts;
	
	// agregate costs horizontally and vertically in the order imposed by horizontalFirst
	for(step = 0; step < 2; ++ step){
		// agregate horizontally
		if(horizontalFirst){
			temporaryCosts = agregateCosts1D(costs, w, h, d, leftBorders, rightBorders, 0, 1);
		}
		// agregate vertically
		else{
			temporaryCosts = agregateCosts1D(costs, w, h, d, upBorders, downBorders, 1, 0);
		}
		for(i = 0; i < w; ++i){
			for(j = 0; j < h; ++j){
				costs[d*h*w+i*h+j] = temporaryCosts[i*h+j];
		horizontalFirst = !horizontalFirst;
	}
}