/* Distance between patches.
    Copyright (C) 2010 Pascal Monasse <monasse@imagine.enpc.fr>

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