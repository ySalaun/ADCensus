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

#ifndef SO_H
#define SO_H

#include "stereoDense/parameters.h"

/// computes the parameters for each pixel cost update during scanline optimization
void scanlineOptimizationParameters(const int x1, const int x2, const int y1, const int y2,
									const int disparity, float &P1, float &P2, const PARAMETERS& params);

/// update the scanline optimization cost when the direction is vertical
/// y1 is current line and y2 is previous line
void scanlineOptimizationV(const float* costs, float* soCosts, 
						   const int y1, const int y2, const PARAMETERS& params);

/// update the scanline optimization cost when the direction is horizontal
/// x1 is current column and x2 is previous column
void scanlineOptimizationH(const float* costs, float* soCosts,
						   const int x1, const int x2, const PARAMETERS& params);

/// compute the so cost by computing a vertical so in the "way" direction with first line of index y0
void scanlineOptimizationComputationV(const float* costs, const int y0, const int way, 
									  float* costSO, const PARAMETERS& params);

/// compute the so cost by computing an horizontal so in the "way" direction with first column of index x0
void scanlineOptimizationComputationH(const float* costs, const int x0, const int way,
									  float* costSO, const PARAMETERS& params);

/// update cost with scanline optimization method:
/// C1(p,d) + min(Cr(p-r,d), Cr(p-r, d+-1) + P1, min_k(Cr(p-r,k)+P2)) - min_k(Cr(p-r,k))
float* scanlineOptimization(const float* costs, const PARAMETERS& params);

#endif