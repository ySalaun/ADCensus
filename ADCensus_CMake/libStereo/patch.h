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

#ifndef PATCH_H
#define PATCH_H

#include "stereoDense/parameters.h"

/// Absolute Differences between pixels (x1,y1) and (x2,y2).
float ad(int x1, int y1, int x2, int y2, const PARAMETERS& params);

/// Census measure between 9x7 patches centered on (x1,y1) and (x2,y2).
/// Computes the Hamming distance between the structures around each pixels
float census(int x1, int y1, int x2, int y2, const PARAMETERS& params);

/// adCensus computes a cost defined by:
/// Cost = (1-exp(COST_AD/lambdaAD)) + (1-exp(COST_CENSUS/lambdaCensus))
/// it is a combination of the cost functions AD and Census explained above
float adCensus(int x1,int y1, int x2,int y2, const PARAMETERS& params);

/// agregateCosts1D computes the agregated costs of a table costs in the direction (dx, dy) and with the borders given in params
/// disparity gives the current disparity
/// it returns a table of the agregated costs
float* agregateCosts1D(float* costs, int disparity, int dx, int dy, const PARAMETERS& params);

/// agregateCosts2D computes the agregated costs of a table costs vertically and horizontally
/// horizontalFirst gives the direction order and the windows borders are given in params
/// disparity gives the current disparity
/// it replaces the costs table by its agregated correspondent
void agregateCosts2D(float* costs, int disparity, bool horizontalFirst, const PARAMETERS& params);

#endif
