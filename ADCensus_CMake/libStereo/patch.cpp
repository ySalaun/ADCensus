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
	const float* p1 = params.im[0].pixel(x1, y1);
	const float* p2 = params.im[1].pixel(x2, y2);
	const float dist = (float)(std::abs((int)(*p1 - *p2)));

	return dist;
}

/// Census measure between 9x7 patches centered on (x1,y1) and (x2,y2).
/// Computes the Hamming distance between the structures around each pixels
float census(int x1, int y1, int x2, int y2, const PARAMETERS& params)
{
	const float p1ref = *(params.im[0].pixel(x1, y1));
	const float p2ref = *(params.im[1].pixel(x2, y2));
	float dist=0.0f;
	for(int y = -params.winY; y <= params.winY; ++y) {
		const float* p1 = params.im[0].pixel(x1-params.winX, y1+y);
		const float* p2 = params.im[1].pixel(x2-params.winX, y2+y);
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
float* agregateCosts1D(float* costs, int dx, int dy, const PARAMETERS& p)
{
	const int offset = p.activePicture*p.w*p.h;

	float* agregatedCosts = new float[p.w*p.h];
	// agregate costs in the direction (dx, dy)
	for(int x=0; x<p.w; ++x)
		for(int y=0; y<p.h; ++y){
            int dmin, dmax;
			// compute agregation window borders
			if(dy == 0){ // horizontal agregation
				dmin = -p.leftBorders[offset+x*p.h+y];
				dmax =  p.rightBorders[offset+x*p.h+y];
			}
			else{ // vertical agregation
				dmin = -p.upBorders[offset+x*p.h+y];
				dmax =  p.downBorders[offset+x*p.h+y];
			}

			// agregate cost
            float c=0;
			for(int d=dmin; d<=dmax; ++d)
				c += costs[(x+d*dx)*p.h+(y+d*dy)];
			agregatedCosts[x*p.h+y] = c;
		}
	return agregatedCosts;
}

/// Compute the agregated costs of a table costs vertically and horizontally.
/// horizontalFirst gives the direction order.
/// Replace the costs table by its agregation.
void agregateCosts2D(float* costs, bool horizontalFirst, const PARAMETERS& p)
{
    int dx=0, dy=1;
    if(horizontalFirst)
        std::swap(dx,dy);

	for(int i=0; i<2; i++) {
        float *temporaryCosts = agregateCosts1D(costs, dx,dy, p);

		// update the costs in costs table
		for(int x=0; x<p.w; ++x)
			for(int y=0; y<p.h; ++y)
				costs[x*p.h+y] = temporaryCosts[x*p.h+y];
        delete [] temporaryCosts;

		// change agregation direction
        std::swap(dx,dy);
	}
}
