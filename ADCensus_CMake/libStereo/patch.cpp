/*  Distance between patches & cost agregation.
    Copyright (C) 2012-2013 Yohann Salaun <yohann.salaun@polytechnique.org

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

/// Absolute Differences between pixels (x1,y1) and (x2,y2).
float ad(int x1, int y1, int x2, int y2, const PARAMETERS& params)
{
	const float* p1 = params.im1.pixel(x1, y1);
	const float* p2 = params.im2.pixel(x2, y2);
	const float dist = (float)(std::abs((int)(*p1 - *p2)));

	return dist;
}

/// Census measure between 9x7 patches centered on (x1,y1) and (x2,y2).
/// Computes the Hamming distance between the structures around each pixels
float census(int x1, int y1, int x2, int y2, const PARAMETERS& params)
{
	const float p1ref = *(params.im1.pixel(x1, y1));
	const float p2ref = *(params.im2.pixel(x2, y2));
	float dist=0.0f;
	for(int y = -params.winY; y <= params.winY; ++y) {
		const float* p1 = params.im1.pixel(x1-params.winX, y1+y);
		const float* p2 = params.im2.pixel(x2-params.winX, y2+y);
		for(int x = -params.winX; x <= params.winX; ++x){
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
float adCensus(int x1,int y1, int x2,int y2, const PARAMETERS& params)
{
	float dist = 0.0f;
	
	// compute Absolute Difference cost
	const float cAD = ad(x1, y1, x2, y2, params);

	// compute Census cost
	const float cCensus = census(x1, y1, x2, y2, params);

	// combine the two costs
	dist += 1-exp(-cAD/params.lambdaAD);
	dist += 1-exp(-cCensus/params.lambdaCensus);

	return dist;
}

/// agregateCosts1D computes the agregated costs of a table costs in the direction (dx, dy) and with the borders given in params
/// disparity gives the current disparity
/// it returns a table of the agregated costs
float* agregateCosts1D(float* costs, int disparity, int dx, int dy, const PARAMETERS& params)
{
	int x, y, d, dmin, dmax;

	const int w = params.w, h = params.h;
	const int offset = (params.activePicture - 1)*w*h;

	float agregatedCost;
	float* agregatedCosts = new float[w*h];
	
	// agregate costs in the direction (dx, dy)
	for(x = 0; x < w; ++x){
		for(y = 0; y < h; ++y){
			// initialize agregated cost
			agregatedCost = 0;
			
			// compute agregation window borders
			// horizontal agregation
			if(dy == 0){
				dmin = -params.leftBorders[offset+x*h+y];
				dmax =  params.rightBorders[offset+x*h+y];
			}
			// vertical agregation
			else{
				dmin = -params.upBorders[offset+x*h+y];
				dmax =  params.downBorders[offset+x*h+y];
			}
			 
			// agregate cost
			for(d = dmin; d <= dmax; ++d){
				agregatedCost += costs[(disparity-params.dMin)*h*w+(x+d*dx)*h+(y+d*dy)];
			}

			// update cost in table
			agregatedCosts[x*params.h+y] = agregatedCost;
		}
	}
	return agregatedCosts;
}

/// agregateCosts2D computes the agregated costs of a table costs vertically and horizontally
/// horizontalFirst gives the direction order and the windows borders are given in params
/// disparity gives the current disparity
/// it replaces the costs table by its agregated correspondent
void agregateCosts2D(float* costs, int disparity, bool horizontalFirst, const PARAMETERS& params)
{
	int step, x, y;

	const int w = params.w, h = params.h;
	const int dMin = params.dMin, dMax = params.dMax;

	float *temporaryCosts;
	
	// agregate costs horizontally and vertically in the order imposed by horizontalFirst
	for(step = 0; step < 2; ++ step){
		// agregate horizontally
		if(horizontalFirst){
			temporaryCosts = agregateCosts1D(costs, disparity, 1, 0, params);
		}
		// agregate vertically
		else{
			temporaryCosts = agregateCosts1D(costs, disparity, 0, 1, params);
		}

		// update the costs in costs table
		for(x = 0; x < w; ++x){
			for(y = 0; y < h; ++y){
				costs[(disparity-dMin)*h*w+x*h+y] = temporaryCosts[x*h+y];
			}
		}

		// change agregation direction
		horizontalFirst = !horizontalFirst;
	}
}