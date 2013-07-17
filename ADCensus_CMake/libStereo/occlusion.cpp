/**
 * @file occlusion.cpp
 * @brief Detect and fill occlusions by left-right consistency
 * @author Pauline Tan <pauline.tan@ens-cachan.fr>
 *         Pascal Monasse <monasse@imagine.enpc.fr>
 * 
 * Copyright (c) 2012-2013, Pauline Tan, Pascal Monasse
 * All rights reserved.
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Pulic License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "occlusion.h"
#include "patch.cpp"
#include <algorithm>
#include <numeric>
#include <vector>
#include <cmath>
#include <cstdlib>

/// Fill pixels below value \a vMin using values at two closest pixels on same
/// line above \a vMin. The filling value is the result of \a cmp with the two
/// values as parameters.
void fillX(LWImage<int>& im, int vMin,
           const int& (*cmp)(const int&,const int&)) {
    for(int y=0; y<im.h; y++) {
        int x0=-1;
        int v0 = vMin;
        while(x0<im.w) {
            int x1=x0+1;
            while(x1<im.w && *im.pixel(x1,y)<vMin) ++x1;
            int v=v0;
            if(x1<im.w)
                v = cmp(v,v0=*im.pixel(x1,y));
            std::fill(im.pixel(x0+1,y), im.pixel(x1,y), v);
            x0=x1;
        }
    }
}

/// Fill pixels below value \a vMin with min of values at closest pixels on same
/// line above \a vMin.
void fillMinX(LWImage<int>& im, int vMin) {
    fillX(im, vMin, std::min<int>);
}

/// Fill pixels below value \a vMin with max of values at closest pixels on same
/// line above \a vMin.
void fillMaxX(LWImage<int>& im, int vMin) {
    fillX(im, vMin, std::max<int>);
}

/// Detect left-right discrepancies in disparity and put incoherent pixels to
/// value \a dOcclusion in \a disparityLeft.
void detect_occlusion(LWImage<int>& disparityLeft,
                      const LWImage<int>& disparityRight,
                      int dOcclusion, int tolDisp) {
    const int w=disparityLeft.w, h=disparityLeft.h;
    for(int y=0; y<h; y++)
        for(int x=0; x<w; x++) {
            int d = *disparityLeft.pixel(x,y);
            if(x+d<0 || x+d>=w || abs(d-*disparityRight.pixel(x+d,y))>tolDisp)
                *disparityLeft.pixel(x,y) = dOcclusion;
        }
}

/// Detect left-right discrepancies in disparity and put incoherent pixels to
/// value \a dOcclusion in \a disparityLeft.
void detect_outliers(LWImage<int>& disparityLeft,
                      const LWImage<int>& disparityRight,
                      int dOcclusion, int dMismatch, int tolDisp,
					  int dMin, int dMax) {
    const int w=disparityLeft.w, h=disparityLeft.h;
    for(int y=0; y<h; y++)
        for(int x=0; x<w; x++) {
            int disparity = *disparityLeft.pixel(x,y);
            // if the point is an outlier, differentiate it between occlusion and mismatch
			if(x+disparity<0 || x+disparity>=w || abs(disparity-*disparityRight.pixel(x+disparity,y))>tolDisp){
				bool occlusion = true;
				for(int d = dMin; d <= dMax; ++d){
					if(x+disparity >= 0 && x+disparity < w && d == *disparityRight.pixel(x+d,y)){
						occlusion = false;
						break;
					}
				}
                *disparityLeft.pixel(x,y) = (occlusion)? dOcclusion:dMismatch;
			}
        }
}

void regionVoting(LWImage<int>& disp, const PARAMETERS& p){
	// temporary disparity map that avoids too fast correction
	LWImage<int>& dispTemp = alloc_image<int>(disp.w,disp.h);

	// histogram for voting
	float* Hp = new float[p.dMax - p.dMin + 1];
	// fill the histogram with 0
	for(int d = p.dMin; d <= p.dMax; ++d){
		Hp[d-p.dMin] = 0;
	}
	// loop on the whole picture
	for(int x=0; x<p.w; ++x){
		for(int y=0; y<p.h; ++y){
			// when the pixel is not an outlier, continue
			if(*disp.pixel(x,y) >= p.dMin){
				*dispTemp.pixel(x,y) = *disp.pixel(x,y);
				continue;
			}
			int leftBorder = -p.leftBorders[x*p.h+y];
			int rightBorder = p.rightBorders[x*p.h+y];
			int nVote = 0;
			for(int xx = leftBorder; xx <= rightBorder; ++xx){
				int upBorder = -p.upBorders[(x+xx)*p.h+y];
				int downBorder = p.downBorders[(x+xx)*p.h+y];
				for(int yy = upBorder; yy <= downBorder; ++yy){
					// if the pixel is an outlier, there is no vote to take into account
					if(*disp.pixel(x+xx, y+yy) < p.dMin){
						continue;
					}
					// increase the number of votes
					++ nVote;
					// update the histogram
					++ Hp[*disp.pixel(x+xx, y+yy) - p.dMin];
				}
			}
			// is the number of vote sufficient
			if(nVote <= p.tauS){
				*dispTemp.pixel(x,y) = *disp.pixel(x,y);
				continue;
			}
			int disparity = *disp.pixel(x,y);
			float voteRatio, voteRatioMax = 0.f;
			for(int d = p.dMin; d <= p.dMax; ++d){
				voteRatio = Hp[d-p.dMin]/nVote;
				if(voteRatio > voteRatioMax){
					voteRatioMax = voteRatio;
					disparity = (voteRatioMax > p.tauH)? d:disparity;
				}
				Hp[d-p.dMin] = 0;
			}
			*dispTemp.pixel(x,y) = disparity;
		}
	}
	std::copy(dispTemp.data, dispTemp.data+dispTemp.w*dispTemp.h, disp.data);
	free(dispTemp.data);
}

void properInterpolation(LWImage<int>& disp, const PARAMETERS& p){
	// temporary disparity map that avoids too fast correction
	LWImage<int>& dispTemp = alloc_image<int>(disp.w,disp.h);
	// look on the 16 different directions
	int directionX[] = {0, 2, 2, 2, 0, -2, -2, -2, 1, 2, 2, 1, -1, -2, -2, -1} ;
	int directionY[] = {2, 2, 0, -2, -2, -2, 0, 2, 2, 1, -1, -2, -2, -1, 1, 2} ;
	// loop on the whole picture
	for(int x=0; x<p.w; ++x){
		for(int y=0; y<p.h; ++y){
			// when the pixel is not an outlier, continue
			if(*disp.pixel(x,y) >= p.dMin){
				*dispTemp.pixel(x,y) = *disp.pixel(x,y);
				continue;
			}
			std::vector<int> disparities(16, *disp.pixel(x,y));
			std::vector<float> cost(16, -1);
			for(int d=0; d<16; ++d){
				int X=x, Y=y;
				for(int step=0; step < 20; ++step){
					// go one step further
					if(step%2 == 0){
						X += directionX[d]/2;
						Y += directionY[d]/2;
					}
					else{
						X += directionX[d] - directionX[d]/2;
						Y += directionY[d] - directionY[d]/2;
					}
					if(X < 0 || X >= p.w || Y < 0 || Y >= p.h){
						break;
					}
					if(*disp.pixel(X,Y) >= p.dMin){
						disparities[d] = *disp.pixel(X,Y);
						cost[d] = ad(x, y, X, Y, p);
						break;
					}
				}
			}
			if(*disp.pixel(x,y) == DISP_OCCLUSION){
				int disparity = disparities[0];
				for(int d=1; d<16; ++d){
					if(disparity > disparities[d]){
						disparity = disparities[d];
					}
				}
				*dispTemp.pixel(x,y) = disparity;
			}
			else{
				int disparity = disparities[0];
				float minCost = cost[0];
				for(int d=1; d<16; ++d){
					if(minCost == -1 || (cost[d] < disparities[d] && cost[d] != -1)){
						disparity = disparities[d];
						minCost = cost[d];
					}
				}
				*dispTemp.pixel(x,y) = disparity;
			}
		}
	}
	std::copy(dispTemp.data, dispTemp.data+dispTemp.w*dispTemp.h, disp.data);
	free(dispTemp.data);
}

/// Square function
inline float sqr(float v1) {
    return v1*v1;
}

/// Square L2 distance between colors at (x1,y1) and at (x2,y2)
float dist2Color(const LWImage<float>& im, int x1,int y1, int x2,int y2) {
    return sqr(*im.pixel(x1,y1       )-*im.pixel(x2,y2       ))+
           sqr(*im.pixel(x1,y1+  im.h)-*im.pixel(x2,y2+  im.h))+
           sqr(*im.pixel(x1,y1+2*im.h)-*im.pixel(x2,y2+2*im.h));
}

/// @brief Compute weighted histogram of image values.
///
/// The area is [x-radius,x+radius]x[y-radius,y+radius] (inter image).
/// Values are shifted by \a vMin.
/// Weights are computed from the \a guidance image with factors \a sSpace for
/// spatial distance and \a sColor for color distance to central pixel.
void weighted_histo(std::vector<float>& tab, int x, int y, int radius,
                    const LWImage<int>& im, int vMin,
                    const LWImage<float>& guidance,
                    float sSpace, float sColor) {
    std::fill(tab.begin(), tab.end(), 0);
    for(int dy=-radius; dy<=radius; dy++)
        if(0<=y+dy && y+dy<im.h)
            for(int dx=-radius; dx<=radius; dx++)
                if(0<=x+dx && x+dx<im.w) {
                    float w =
                        exp(-(dx*dx+dy*dy)*sSpace
                            -dist2Color(guidance, x,y,x+dx,y+dy)*sColor);
                    tab[*im.pixel(x+dx,y+dy)-vMin] += w;
                }
}

/// Index in histogram \a tab reaching median.
static int median_histo(const std::vector<float>& tab) {
    float sum=std::accumulate(tab.begin(), tab.end(), 0.0f)/2;
    int d=-1;
    for(float cumul=0; cumul<sum;)
        cumul += tab[++d];
    return d;
}

/// @brief Weighted median filter of current image.
///
/// Image is assumed to have integer values in [vMin,vMax]. Weight are computed
/// as in bilateral filter in color image \a guidance. Only pixels of image
/// \a where outside [vMin,vMax] are filtered.
void weightedMedianColor(const LWImage<int>& dispDense,
                         const LWImage<float>& guidance,
                         LWImage<int>& where, int vMin, int vMax,
                         int radius, float sSpace, float sColor) {
    sSpace = 1.0f/(sSpace*sSpace);
    sColor = 1.0f/(sColor*sColor);

    const int size=vMax-vMin+1;
    std::vector<float> tab(size);

#ifdef _OPENMP
#pragma omp parallel for firstprivate(tab)
#endif
    for(int y=0; y<where.h; y++)
        for(int x=0; x<where.w; x++) {
            if(vMin<=*where.pixel(x,y) && *where.pixel(x,y)<=vMax)
                continue;
            weighted_histo(tab, x,y, radius,
                           dispDense, vMin, guidance, sSpace, sColor);
            *where.pixel(x,y) = vMin+median_histo(tab);
        }
}

/// Fill occlusions by weighted median filtering.
///
/// \param dispDense Disparity image
/// \param guidance Color guidance image, where weights are computed
/// \param disparity Values outside [dispMin,dispMax] are interpolated
/// \param dispMin,dispMax Min/max disparities
/// \param paramOcc Parameters to compute weights in bilateral filtering
void fill_occlusion(const LWImage<int>& dispDense,
                    const LWImage<float>& guidance,
                    LWImage<int>& disparity, int dispMin, int dispMax,
                    const ParamOcclusion& paramOcc) {
    weightedMedianColor(dispDense, guidance,
                        disparity, dispMin, dispMax, 
                        paramOcc.median_radius,
                        paramOcc.sigma_space,
                        paramOcc.sigma_color);
}
