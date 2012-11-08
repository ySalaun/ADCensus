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

#ifndef PATCH_H
#define PATCH_H

#include "libLWImage/LWImage.h"

float      sum(const LWImage<float>& im, int i, int j,  int win);
float      ssd(const LWImage<float>& im1,int i1,int j1,
               const LWImage<float>& im2,int i2,int j2, int win);
float     cssd(const LWImage<float>& im1,int i1,int j1,
               const LWImage<float>& im2,int i2,int j2, int win);
float       ad(const LWImage<float>& im1, int i1,int j1, 
               const LWImage<float>& im2, int i2,int j2);
float   census(const LWImage<float>& im1, int i1,int j1, 
		   	   const LWImage<float>& im2, int i2,int j2, const int winI = 9, const int winJ = 7);
float adCensus(const LWImage<float>& im1, int i1,int j1, 
			   const LWImage<float>& im2, int i2,int j2, const int winI = 9, const int winJ = 7,
			   const float lambdaAD = 10, const float lambdaCensus = 30);
int patchBorder(const LWImage<float>& im, const int i, const int j,
				const int di, const int dj,
				const int l1, const int l2, const int tau1, const int tau2);
int* patchesBorder(const LWImage<float>& im, const int di, const int dj,
				   const int l1, const int l2, const int tau1, const int tau2);
float* agregateCosts1D(float* costs, int w, int h, int d,
					   const int* leftBorders, const int* rightBorders,
					   int di, int dj);
void agregateCosts2D(float* costs, int w, int h, int d,
					 const int* leftBorders, const int* rightBorders,
					 const int* upBorders, const int* downBorders,
					 bool horizontalFirst);
void scanlineOptimizationParameters(const LWImage<float>& im1, const LWImage<float>& im2,
									const int i1, const int i2, const int j1, const int j2, const int disparity,
									const float pi1, const float pi2, const int tauSO,
									float &P1, float &P2);
// costs = Cr
// soCosts = C1
// w & h = width & height of picture
// dMin & dMax = disparity range
// j1 column for so, j2 previous line
void scanlineOptimizationV(const LWImage<float>& im1, const LWImage<float>& im2,
						   const float* costs, float* soCosts,
						   const int dMin, const int dMax, const int j1, const int j2,
						   const float pi1, const float pi2, const int tauSO);
// costs = Cr
// soCosts = C1
// w & h = width & height of picture
// dMin & dMax = disparity range
// i1 line for so, i2 previous column
void scanlineOptimizationH(const LWImage<float>& im1, const LWImage<float>& im2,
						   const float* costs, float* soCosts,
						   const int dMin, const int dMax, const int i1, const int i2,
						   const float pi1, const float pi2, const int tauSO);
// compute the so cost by computing a vertical so
void scanlineOptimizationComputationV(const LWImage<float>& im1, const LWImage<float>& im2,
									  const float* costs, const int dMin, const int dMax,
									  const float pi1, const float pi2, const int tauSO,
									  const int j0, const int way, float* costSO);
// compute the so cost by computing an horizontal so
void scanlineOptimizationComputationH(const LWImage<float>& im1, const LWImage<float>& im2,
									  const float* costs, const int dMin, const int dMax,
									  const float pi1, const float pi2, const int tauSO,
									  const int i0, const int way, float* costSO);
// C1(p,d) + min(Cr(p-r,d), Cr(p-r, d+-1) + P1, min_k(Cr(p-r,k)+P2)) - min_k(Cr(p-r,k))
float* scanlineOptimization(const LWImage<float>& im1, const LWImage<float>& im2,
							const float* costs, const int dMin, const int dMax,
							const float pi1 = 1.0, const float pi2 = 3.0,
							const int tauSO = 15);
#endif
