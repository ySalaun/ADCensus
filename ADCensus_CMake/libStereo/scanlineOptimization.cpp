/*  Scanline optimization.
    Copyright (C) 2012-2013 Yohann Salaun <yohann.salaun@polytechnique.org>

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

#include "scanlineOptimization.h"
#include <cmath>
#include <algorithm>

/// Compute the color difference of 2 pixels
int colorDiff(const float* p1, const float* p2, const int wh){
	int d, diff = 0;
	for(int color = 0; color < 3; ++color){
		d = abs(int(*(p1+color*wh)-*(p2+color*wh)));
		diff = (diff > d)? diff:d;
	}
	return diff;
}

/// computes the parameters for each pixel cost update during scanline optimization
void scanlineOptimizationParameters(int x1, int x2, int y1, int y2,	int d,
                                    float& P1, float& P2,
                                    const PARAMETERS& p)
{
    int a=p.activePicture, b=1-a;
    if(a!=0) d=-d;

    // compute color differences between pixels in the two pictures
	int d1 = colorDiff(p.im[a].pixel(x1,y1), p.im[a].pixel(x2,y2), p.wh) ,d2 = 0;
    if(0<=x1+d && x1+d<p.w && 0<=x2+d && x2+d<p.w)
       d2 = colorDiff(p.im[b].pixel(x1+d,y1), p.im[b].pixel(x2+d,y2), p.wh);

    // depending on the color differences computed previously, find the so parameters
    if(d1 < p.tauSO){
		if(d2 < p.tauSO){
			P1 = p.pi1;
			P2 = p.pi2;
		}
		else{
			P1 = float(p.pi1/4.0);
			P2 = float(p.pi2/4.0);
		}
	}
	else {
		if(d2 < p.tauSO){
			P1 = float(p.pi1/4.0);
			P2 = float(p.pi2/4.0);
		}
		else {
			P1 = float(p.pi1/10.0);
			P2 = float(p.pi2/10.0);
		}
	}
}

void optim(const float* costs, float* soCosts,
           int x1, int x2, int y1, int y2, const PARAMETERS& params) {
	const int w=params.w, h=params.h;
	const int dMin = params.dMin, dMax = params.dMax;

    float minCost = soCosts[0*h*w+x2*h+y2];
    // find minimal previous cost for a given column index
    for(int d=dMin+1; d<=dMax; ++d)
        if(minCost > soCosts[(d-dMin)*h*w+x2*h+y2])
            minCost = soCosts[(d-dMin)*h*w+x2*h+y2];

    for(int d=dMin; d<=dMax; ++d) {
        // C1(p,d) - min_k(Cr(p-r,k))
        float soCost = costs[(d-dMin)*h*w+x1*h+y1]-minCost;

        // compute P1 and P2 parameters for better scanline optimization
        float P1, P2;
        scanlineOptimizationParameters(x1, x2, y1, y2, d, P1, P2, params);

        // compute min(Cr(p-r,d), Cr(p-r, d+-1) + P1, min_k(Cr(p-r,k)+P2)) 
        minCost += P2;
        if(minCost > soCosts[(d-dMin)*h*w+x2*h+y2])
            minCost = soCosts[(d-dMin)*h*w+x2*h+y2];
        if(d != params.dMin && minCost > soCosts[(d-dMin-1)*h*w+x2*h+y2] + P1)
            minCost = soCosts[(d-dMin-1)*h*w+x2*h+y2] + P1;
        if(d != params.dMax && minCost > soCosts[(d-dMin+1)*h*w+x2*h+y2] + P1)
            minCost = soCosts[(d-dMin+1)*h*w+x2*h+y2] + P1;

        soCost += minCost;
        soCosts[(d-dMin)*h*w+x1*h+y1] = soCost;
    }
}

/// update the scanline optimization cost when the direction is vertical
/// y1 is current line and y2 is previous line
void scanlineOptimizationV(const float* costs, float* soCosts, 
						   int y1, int y2, const PARAMETERS& params)
{
	for(int x=0; x<params.w; ++x)
        optim(costs, soCosts, x, x, y1, y2, params);
}

/// update the scanline optimization cost when the direction is horizontal
/// x1 is current column and x2 is previous column
void scanlineOptimizationH(const float* costs, float* soCosts,
						   int x1, int x2, const PARAMETERS& params)
{
	for(int y=0; y<params.h; ++y)
        optim(costs, soCosts, x1, x2, y, y, params);
}

/// compute the so cost by computing a vertical so in the "way" direction with first line of index y0
void scanlineOptimizationComputationV(const float* costs, int y0, int way, 
									  float* costSO, const PARAMETERS& params)
{
	const int h = params.h, w = params.w;
	const int dMin = params.dMin, dMax = params.dMax;
	
	float *tempor = new float[w*h*(dMax-dMin+1)];

	// initialize vertical so cost
	for(int x=0; x<w; ++x)
		for(int d=dMin; d<=dMax; ++d)
			tempor[(d-dMin)*h*w+x*h+y0] = costs[(d-dMin)*h*w+x*h+y0];

	// computes vertical so cost
	for(int y=y0+way; 0<=y && y<h; y+=way)
		scanlineOptimizationV(costs, tempor, y, y-way, params);

	// add so cost into costSO table
	for(int x=0; x<w; ++x)
		for(int y=0; y<h; ++y)
			for(int d=dMin; d<=dMax; ++d)
				costSO[(d-dMin)*h*w+x*h+y] += tempor[(d-dMin)*h*w+x*h+y];
    delete [] tempor;
}

/// compute the so cost by computing an horizontal so in the "way" direction with first column of index x0
void scanlineOptimizationComputationH(const float* costs, int x0, int way,
									  float* costSO, const PARAMETERS& params)
{
	const int h = params.h, w = params.w;
	const int dMin = params.dMin, dMax = params.dMax;

	float *tempor = new float[w*h*(dMax-dMin+1)];

	// initialize horizontal so cost
	for(int y=0; y<h; ++y)
		for(int d=dMin; d<=dMax; ++d)
			tempor[(d-dMin)*h*w+x0*h+y] = costs[(d-dMin)*h*w+x0*h+y];

	// computes horizontal so cost
	for(int x=x0+way; 0<=x && x<w; x+=way)
		scanlineOptimizationH(costs, tempor, x, x-way, params);

	// add so cost into costSO table
	for(int x=0; x<w; ++x)
		for(int y=0; y<h; ++y)
			for(int d=dMin; d<=dMax; ++d)
				costSO[(d-dMin)*h*w+x*h+y] += tempor[(d-dMin)*h*w+x*h+y];
    delete [] tempor;
}

/// update cost with scanline optimization method:
/// C1(p,d) + min(Cr(p-r,d), Cr(p-r, d+-1) + P1, min_k(Cr(p-r,k)+P2)) - min_k(Cr(p-r,k))
float* scanlineOptimization(const float* costs, const PARAMETERS& params)
{
	const int h = params.h, w = params.w;
	const int dMin = params.dMin, dMax = params.dMax;

    int size = w*h*(dMax-dMin+1);
	float *costSO = new float[size];
    std::fill(costSO, costSO+size, 0);

	// computes the 4 directionnal so costs
	// vertical downward
	scanlineOptimizationComputationV(costs,   0,  1, costSO, params);
	// vertical upward
	scanlineOptimizationComputationV(costs, h-1, -1, costSO, params);
	// horizontal leftward
	scanlineOptimizationComputationH(costs,   0,  1, costSO, params);
	// horizontal rightward
	scanlineOptimizationComputationH(costs, w-1, -1, costSO, params);
	
	return costSO;
}
