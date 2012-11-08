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
#include <iostream>

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
				const int l1, const int l2, const int tau1, const int tau2)
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
		bool condition3 = d <= l2 || (d > l2 && abs(p1 - p) < tau2);

		// if conditions are checked, go further in (di, dj) direction
		if(condition1 && condition2 && condition3){
			++ d;
			i1 = i + d*di;
			j1 = j + d*dj;
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
				   const int l1, const int l2, const int tau1, const int tau2)
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
					   const int* leftBorders, const int* rightBorders,
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
					 const int* leftBorders, const int* rightBorders,
					 const int* upBorders, const int* downBorders,
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
			}
		}
		horizontalFirst = !horizontalFirst;
	}
}

void scanlineOptimizationParameters(const LWImage<float>& im1, const LWImage<float>& im2,
									const int i1, const int i2, const int j1, const int j2, const int disparity,
									const float pi1, const float pi2, const int tauSO,
									float &P1, float &P2)
{

	int h = im1.h;
	int w = im2.w;
	int d1, d2;

	d1 = int(abs(*(im1.pixel(i1, j1)) - *(im1.pixel(i2, j2))));
	if(i1 + disparity < w && i1 + disparity >= 0 && i2 + disparity < w && i2 + disparity >= 0){
		d2 = int(abs(*(im1.pixel(i1 + disparity, j1)) - *(im2.pixel(i2 + disparity, j2))));
	}
	else{
		d2 = 0;
	}
	if(d1 < tauSO){
		if(d2 < tauSO){
			P1 = pi1;
			P2 = pi2;
		}
		else{
			P1 = pi1/4;
			P2 = pi2/4;
		}
	}
	else{
		if(d2 < tauSO){
			P1 = pi1/4;
			P2 = pi2/4;
		}
		else{
			P1 = pi1/10;
			P2 = pi2/10;
		}
	}
}

// costs = Cr
// soCosts = C1
// w & h = width & height of picture
// dMin & dMax = disparity range
// j1 column for so, j2 previous line
void scanlineOptimizationV(const LWImage<float>& im1, const LWImage<float>& im2,
						   const float* costs, float* soCosts,
						   const int dMin, const int dMax, const int j1, const int j2,
						   const float pi1, const float pi2, const int tauSO)
{
	int i, disparity;
	float minCost, soCost, P1, P2;
	const int h = im1.h, w = im1.w;

	for(i = 0; i < w; ++i){
		minCost = costs[0*h*w+i*h+j1];
		// find minimal previous cost for a given column index
		for(disparity = dMin+1; disparity <= dMax; ++disparity){
			if(minCost > costs[(disparity-dMin)*h*w+i*h+j2]){
				minCost = (soCosts[(disparity-dMin)*h*w+i*h+j2]);
			}
		}
		for(disparity = dMin; disparity <= dMax; ++disparity){
			soCost = costs[(disparity-dMin)*h*w+i*h+j1];

			// compute P1 and P2 parameters for better scanline optimization
			scanlineOptimizationParameters(im1, im2, i, i, j1, j2, disparity, pi1, pi2, tauSO, P1, P2);

			// substract by min_k(Cr(p-r,k))
			soCost -= minCost;

			// compute min(Cr(p-r,d), Cr(p-r, d+-1) + P1, min_k(Cr(p-r,k)+P2)) 
			minCost += P2;
			if(minCost > costs[(disparity-dMin)*h*w+i*h+j2]){
				minCost = soCosts[(disparity-dMin)*h*w+i*h+j2];
			}
			if(disparity != dMin && minCost > costs[(disparity-dMin-1)*h*w+i*h+j2] + P1){
				minCost = soCosts[(disparity-dMin-1)*h*w+i*h+j2] + P1;
			}
			if(disparity != dMax && minCost > costs[(disparity-dMin+1)*h*w+i*h+j2] + P1){
				minCost = soCosts[(disparity-dMin+1)*h*w+i*h+j2] + P1;
			}

			soCost += minCost;
			soCosts[(disparity-dMin)*h*w+i*h+j1] = soCost;
		}
	}
}

// costs = Cr
// soCosts = C1
// w & h = width & height of picture
// dMin & dMax = disparity range
// i1 line for so, i2 previous column
void scanlineOptimizationH(const LWImage<float>& im1, const LWImage<float>& im2,
						   const float* costs, float* soCosts,
						   const int dMin, const int dMax, const int i1, const int i2,
						   const float pi1, const float pi2, const int tauSO)
{
	int j, disparity;
	float minCost, soCost, P1, P2;
	const int h = im1.h, w = im1.w;

	for(j = 0; j < h; ++j){
		minCost = costs[0*h*w+i1*h+j];
		// find minimal previous cost for a given line index
		for(disparity = dMin+1; disparity <= dMax; ++disparity){
			if(minCost > costs[(disparity-dMin)*h*w+i2*h+j]){
				minCost = (soCosts[(disparity-dMin)*h*w+i2*h+j]);
			}
		}
		for(disparity = dMin; disparity <= dMax; ++disparity){
			soCost = costs[(disparity-dMin)*h*w+i1*h+j];

			// compute P1 and P2 parameters for better scanline optimization
			scanlineOptimizationParameters(im1, im2, i1, i2, j, j, disparity, pi1, pi2, tauSO, P1, P2);

			// substract by min_k(Cr(p-r,k))
			soCost -= minCost;

			// compute min(Cr(p-r,d), Cr(p-r, d+-1) + P1, min_k(Cr(p-r,k)+P2)) 
			minCost += P2;
			if(minCost > costs[(disparity-dMin)*h*w+i2*h+j]){
				minCost = soCosts[(disparity-dMin)*h*w+i2*h+j];
			}
			if(disparity != dMin && minCost > costs[(disparity-dMin-1)*h*w+i2*h+j] + P1){
				minCost = soCosts[(disparity-dMin-1)*h*w+i2*h+j] + P1;
			}
			if(disparity != dMax && minCost > costs[(disparity-dMin+1)*h*w+i2*h+j] + P1){
				minCost = soCosts[(disparity-dMin+1)*h*w+i2*h+j] + P1;
			}

			soCost += minCost;
			soCosts[(disparity-dMin)*h*w+i1*h+j] = soCost;
		}
	}
}

// compute the so cost by computing a vertical so
void scanlineOptimizationComputationV(const LWImage<float>& im1, const LWImage<float>& im2,
									  const float* costs, const int dMin, const int dMax,
									  const float pi1, const float pi2, const int tauSO,
									  const int j0, const int way, float* costSO)
{
	int i, j, disparity;
	const int h = im1.h, w = im1.w;
	float *tempor = new float[w*h*(dMax-dMin+1)];

	// initialize vertical so cost
	for(i = 0; i < w; ++i){
		for(disparity = dMin; disparity <= dMax; ++disparity){
			tempor[(disparity-dMin)*h*w+i*h+j0] = costs[(disparity-dMin)*h*w+i*h+j0];
		}
	}

	// computes vertical so cost
	for(j = j0 + way; j < h && j >= 0; j += way){
		scanlineOptimizationV(im1, im2, costs, tempor, dMin, dMax, j, j-way, pi1, pi2, tauSO);
	}

	// add so cost into costSO table
	for(i = 0; i < w; ++i){
		for(j = 0; j < h; ++j){
			for(disparity = dMin; disparity <= dMax; ++disparity){
				costSO[(disparity-dMin)*h*w+i*h+j] += tempor[(disparity-dMin)*h*w+i*h+j];
			}
		}
	}
}


// compute the so cost by computing an horizontal so
void scanlineOptimizationComputationH(const LWImage<float>& im1, const LWImage<float>& im2,
									  const float* costs, const int dMin, const int dMax,
									  const float pi1, const float pi2, const int tauSO,
									  const int i0, const int way, float* costSO)
{
	int i, j, disparity;
	const int h = im1.h, w = im1.w;
	float *tempor = new float[w*h*(dMax-dMin+1)];

	// initialize horizontal so cost
	for(j = 0; j < h; ++j){
		for(disparity = dMin; disparity <= dMax; ++disparity){
			tempor[(disparity-dMin)*h*w+i0*h+j] = costs[(disparity-dMin)*h*w+i0*h+j];
		}
	}

	// computes horizontal so cost
	for(i = i0 + way; i < w && i >= 0; i += way){
		scanlineOptimizationH(im1, im2, costs, tempor, dMin, dMax, i, i-way, pi1, pi2, tauSO);
	}

	// add so cost into costSO table
	for(i = 0; i < w; ++i){
		for(j = 0; j < h; ++j){
			for(disparity = dMin; disparity <= dMax; ++disparity){
				costSO[(disparity-dMin)*h*w+i*h+j] += tempor[(disparity-dMin)*h*w+i*h+j];
			}
		}
	}
}



// C1(p,d) + min(Cr(p-r,d), Cr(p-r, d+-1) + P1, min_k(Cr(p-r,k)+P2)) - min_k(Cr(p-r,k))
float* scanlineOptimization(const LWImage<float>& im1, const LWImage<float>& im2,
							const float* costs, const int dMin, const int dMax,
							const float pi1, const float pi2, const int tauSO)
{
	int i, j, disparity;
	const int h = im1.h, w = im1.w;

	float *costSO = new float[w*h*(dMax-dMin+1)];
	
	// initialize so cost
	for(i = 0; i < w; ++i){
		for(j = 0; j < h; ++j){
			for(disparity = dMin; disparity <= dMax; ++disparity){
				costSO[(disparity-dMin)*h*w+i*h+j] = 0;
			}
		}
	}

	// computes the 4 directionnal so costs
	// vertical downward
	scanlineOptimizationComputationV(im1, im2, costs, dMin, dMax, pi1, pi2, tauSO, 0, 1, costSO);
	// vertical upward
	scanlineOptimizationComputationV(im1, im2, costs, dMin, dMax, pi1, pi2, tauSO, h-1, -1, costSO);
	// horizontal leftward
	scanlineOptimizationComputationH(im1, im2, costs, dMin, dMax, pi1, pi2, tauSO, 0, 1, costSO);
	// horizontal rightward
	scanlineOptimizationComputationH(im1, im2, costs, dMin, dMax, pi1, pi2, tauSO, w-1, 1, costSO);
	
	return costSO;
}