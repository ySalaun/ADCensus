/*  Scanline optimization.
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

#include "scanlineOptimization.h"
#include "math.h"

/// computes the parameters for each pixel cost update during scanline optimization
void scanlineOptimizationParameters(const int x1, const int x2, const int y1, const int y2,
									const int disparity, float &P1, float &P2, const PARAMETERS& params)
{
	int h = params.h;
	int w = params.w;
	int d1, d2;

	d1 = int(abs(*(params.im1.pixel(x1, y1)) - *(params.im1.pixel(x2, y2))));
	if(x1 + disparity < w && x1 + disparity >= 0 && x2 + disparity < w && x2 + disparity >= 0){
		d2 = int(abs(*(params.im2.pixel(x1 + disparity, y1)) - *(params.im2.pixel(x2 + disparity, y2))));
	}
	else{
		d2 = 0;
	}
	if(d1 < params.tauSO){
		if(d2 < params.tauSO){
			P1 = params.pi1;
			P2 = params.pi2;
		}
		else{
			P1 = float(params.pi1/4.0);
			P2 = float(params.pi2/4.0);
		}
	}
	else{
		if(d2 < params.tauSO){
			P1 = float(params.pi1/4.0);
			P2 = float(params.pi2/4.0);
		}
		else{
			P1 = float(params.pi1/10.0);
			P2 = float(params.pi2/10.0);
		}
	}
}

/// update the scanline optimization cost when the direction is vertical
/// y1 is current line and y2 is previous line
void scanlineOptimizationV(const float* costs, float* soCosts, 
						   const int y1, const int y2, const PARAMETERS& params)
{
	int x, disparity;
	float minCost, soCost, P1, P2;

	const int h = params.h, w = params.w;
	const int dMin = params.dMin, dMax = params.dMax;

	for(x = 0; x < w; ++x){
		minCost = costs[0*h*w+x*h+y1];
		// find minimal previous cost for a given column index
		for(disparity = dMin+1; disparity <= dMax; ++disparity){
			if(minCost > costs[(disparity-dMin)*h*w+x*h+y2]){
				minCost = (soCosts[(disparity-dMin)*h*w+x*h+y2]);
			}
		}
		for(disparity = dMin; disparity <= dMax; ++disparity){
			soCost = costs[(disparity-dMin)*h*w+x*h+y1];

			// compute P1 and P2 parameters for better scanline optimization
			scanlineOptimizationParameters(x, x, y1, y2, disparity, P1, P2, params);

			// substract by min_k(Cr(p-r,k))
			soCost -= minCost;

			// compute min(Cr(p-r,d), Cr(p-r, d+-1) + P1, min_k(Cr(p-r,k)+P2)) 
			minCost += P2;
			if(minCost > costs[(disparity-dMin)*h*w+x*h+y2]){
				minCost = soCosts[(disparity-dMin)*h*w+x*h+y2];
			}
			if(disparity != params.dMin && minCost > costs[(disparity-dMin-1)*h*w+x*h+y2] + P1){
				minCost = soCosts[(disparity-dMin-1)*h*w+x*h+y2] + P1;
			}
			if(disparity != params.dMax && minCost > costs[(disparity-dMin+1)*h*w+x*h+y2] + P1){
				minCost = soCosts[(disparity-dMin+1)*h*w+x*h+y2] + P1;
			}

			soCost += minCost;
			soCosts[(disparity-dMin)*h*w+x*h+y1] = soCost;
		}
	}
}

/// update the scanline optimization cost when the direction is horizontal
/// x1 is current column and x2 is previous column
void scanlineOptimizationH(const float* costs, float* soCosts,
						   const int x1, const int x2, const PARAMETERS& params)
{
	int y, disparity;
	float minCost, soCost, P1, P2;

	const int h = params.h, w = params.w;
	const int dMin = params.dMin, dMax = params.dMax;

	for(y = 0; y < h; ++y){
		minCost = costs[0*h*w+x1*h+y];
		// find minimal previous cost for a given line index
		for(disparity = dMin+1; disparity <= dMax; ++disparity){
			if(minCost > costs[(disparity-dMin)*h*w+x2*h+y]){
				minCost = (soCosts[(disparity-dMin)*h*w+x2*h+y]);
			}
		}
		for(disparity = dMin; disparity <= dMax; ++disparity){
			soCost = costs[(disparity-dMin)*h*w+x1*h+y];

			// compute P1 and P2 parameters for better scanline optimization
			scanlineOptimizationParameters(x1, x2, y, y, disparity, P1, P2, params);

			// substract by min_k(Cr(p-r,k))
			soCost -= minCost;

			// compute min(Cr(p-r,d), Cr(p-r, d+-1) + P1, min_k(Cr(p-r,k)+P2)) 
			minCost += P2;
			if(minCost > costs[(disparity-dMin)*h*w+x2*h+y]){
				minCost = soCosts[(disparity-dMin)*h*w+x2*h+y];
			}
			if(disparity != dMin && minCost > costs[(disparity-dMin-1)*h*w+x2*h+y] + P1){
				minCost = soCosts[(disparity-dMin-1)*h*w+x2*h+y] + P1;
			}
			if(disparity != dMax && minCost > costs[(disparity-dMin+1)*h*w+x2*h+y] + P1){
				minCost = soCosts[(disparity-dMin+1)*h*w+x2*h+y] + P1;
			}

			soCost += minCost;
			soCosts[(disparity-dMin)*h*w+x1*h+y] = soCost;
		}
	}
}

/// compute the so cost by computing a vertical so in the "way" direction with first line of index y0
void scanlineOptimizationComputationV(const float* costs, const int y0, const int way, 
									  float* costSO, const PARAMETERS& params)
{
	int x, y, disparity;

	const int h = params.h, w = params.w;
	const int dMin = params.dMin, dMax = params.dMax;
	
	float *tempor = new float[w*h*(dMax-dMin+1)];

	// initialize vertical so cost
	for(x = 0; x < w; ++x){
		for(disparity = dMin; disparity <= dMax; ++disparity){
			tempor[(disparity-dMin)*h*w+x*h+y0] = costs[(disparity-dMin)*h*w+x*h+y0];
		}
	}

	// computes vertical so cost
	for(y = y0 + way; y < h && y >= 0; y += way){
		scanlineOptimizationV(costs, tempor, y, y-way, params);
	}

	// add so cost into costSO table
	for(x = 0; x < w; ++x){
		for(y = 0; y < h; ++y){
			for(disparity = dMin; disparity <= dMax; ++disparity){
				costSO[(disparity-dMin)*h*w+x*h+y] += tempor[(disparity-dMin)*h*w+x*h+y];
			}
		}
	}
}

/// compute the so cost by computing an horizontal so in the "way" direction with first column of index x0
void scanlineOptimizationComputationH(const float* costs, const int x0, const int way,
									  float* costSO, const PARAMETERS& params)
{
	int x, y, disparity;

	const int h = params.h, w = params.w;
	const int dMin = params.dMin, dMax = params.dMax;
	float *tempor = new float[w*h*(dMax-dMin+1)];

	// initialize horizontal so cost
	for(y = 0; y < h; ++y){
		for(disparity = dMin; disparity <= dMax; ++disparity){
			tempor[(disparity-dMin)*h*w+x0*h+y] = costs[(disparity-dMin)*h*w+x0*h+y];
		}
	}

	// computes horizontal so cost
	for(x = x0 + way; x < w && x >= 0; x += way){
		scanlineOptimizationH(costs, tempor, x, x-way, params);
	}

	// add so cost into costSO table
	for(x = 0; x < w; ++x){
		for(y = 0; y < h; ++y){
			for(disparity = dMin; disparity <= dMax; ++disparity){
				costSO[(disparity-dMin)*h*w+x*h+y] += tempor[(disparity-dMin)*h*w+x*h+y];
			}
		}
	}
}

/// update cost with scanline optimization method:
/// C1(p,d) + min(Cr(p-r,d), Cr(p-r, d+-1) + P1, min_k(Cr(p-r,k)+P2)) - min_k(Cr(p-r,k))
float* scanlineOptimization(const float* costs, const PARAMETERS& params)
{
	int x, y, disparity;

	const int h = params.h, w = params.w;
	const int dMin = params.dMin, dMax = params.dMax;

	float *costSO = new float[w*h*(dMax-dMin+1)];
	
	// initialize so cost
	for(x = 0; x < w; ++x){
		for(y = 0; y < h; ++y){
			for(disparity = dMin; disparity <= dMax; ++disparity){
				costSO[(disparity-dMin)*h*w+x*h+y] = 0;
			}
		}
	}

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