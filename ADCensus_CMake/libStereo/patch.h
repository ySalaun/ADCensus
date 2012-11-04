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
#endif
