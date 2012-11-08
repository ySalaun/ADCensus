#include "libStereo/patch.h"
#include "libIO/io_png.h"
#include "libIO/io_tiff.h"
#include "libIO/nan.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <cfloat>

/// Ignore contrast, that is remove first PC.
static const bool bIgnoreContrast=true;
/// Radius of patch for correlation.
static const int win=9;

static bool loadImage(const char* name, LWImage<float>& im) {
    size_t nx, ny;
    im.data = read_png_f32_gray(name, &nx, &ny);
    im.w = nx; im.h = ny;
    if(! im.data)
        std::cerr << "Unable to load image file " << name << std::endl;
    return (im.data!=0);
}

/// Compute disparity map from \a im1 to \a im2.
// TODO change value of win into winX & winY and uses the values 9 & 7
// TODO with agregation & census windows, borders are more and more truncated....
static void disparity(LWImage<float> im1, LWImage<float> im2,
                      int dMin, int dMax, int win,
                      LWImage<float>& disp, LWImage<float>* costMap) 
{
    /*float (*dispFunction)(const LWImage<float>&,int,int,
                          const LWImage<float>&,int,int, int)=
        (bIgnoreContrast? cssd: ssd);
    std::fill_n(disp.data, disp.w*disp.h, NaN);
    if(costMap)
        std::fill_n(costMap->data, costMap->w*costMap->h, NaN);
    const int ysup = std::min(im1.h, im2.h)-win;
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for(int y=win; y<ysup; y++)
        for(int x=win; x+win<im1.w; x++) {
            float lowCost = FLT_MAX;
            int dBest=dMin-1;
            for(int d=dMin; d<=dMax; d++)
                if(win<=x+d && x+d+win<im2.w) {
                    //float cost = dispFunction(im1,x,y, im2,x+d,y, win);
                    float cost = adCensus(im1,x,y, im2,x+d,y);
					if(cost < lowCost) {
                        lowCost = cost;
                        dBest = d;
                    }
                }
            if(dBest>=dMin) {
                *disp.pixel(x,y) = static_cast<float>(dBest);
                if(costMap)
                    *costMap->pixel(x,y) = lowCost;
            }
        }*/
	int i, j, disparity, step;
	bool horizontalFirst;

	// parameters
	int w = im1.w, h = im1.h;					// width and height of pictures
	int l1 = 34, l2 = 17, tau1 = 20, tau2 = 6;	// for adaptative window
	int nbAgregatingIteration = 4;				// for cost agregation
	
	// tables
	float *costs;
	float *lowCosts;
	int *upBorders, *downBorders, *leftBorders, *rightBorders;
	costs = new float[h*w*(dMax-dMin+1)];
	lowCosts = new float[h*w];
	
	// compute borders for adaptative windows
	upBorders    = patchesBorder(im1, -1,  0, l1, l2, tau1, tau2);
	downBorders  = patchesBorder(im1,  1,  0, l1, l2, tau1, tau2);
	leftBorders  = patchesBorder(im1,  0, -1, l1, l2, tau1, tau2);
	rightBorders = patchesBorder(im1,  0,  1, l1, l2, tau1, tau2);
	
	// computes ad census costs for each pixel and each disparity
	for(i = 0; i < w; ++i){
		for(j = 0; j < h; ++j){
            for(disparity = dMin; disparity <= dMax; ++disparity){
				if(i + disparity < win || i + disparity >= w - win || i < win || i >= w-win || j < win || j > h-win){
					costs[(disparity-dMin)*h*w+i*h+j] = 5;
				}
				else{
					costs[(disparity-dMin)*h*w+i*h+j] = adCensus(im1,i,j, im2,i+disparity,j);
				}
			}
		}
	}
	
	// agregate cost for each pichel in its adaptive window for each possible value of disparity
	for(disparity = 0; disparity <= dMax-dMin; ++disparity){
		horizontalFirst = true;
		for(step = 0; step < nbAgregatingIteration; ++step){
			agregateCosts2D(costs, w, h, disparity, leftBorders, rightBorders, upBorders, downBorders, horizontalFirst);
			horizontalFirst = !horizontalFirst;
		}
	}

	// compute scanline optimization
	// TODO

	// find lowest cost and explicit disparity
	// initialize lowCosts table and disparity table
	for(i = 0; i < w; ++i){
		for(j = 0; j < h; ++j){
			lowCosts[i*h+j] = costs[i*h+j];
			*disp.pixel(i,j) = static_cast<float>(dMin);
		}
	}
	// find best disparity
	for(disparity = dMin+1; disparity <= dMax; ++disparity){
		for(i = 0; i < w; ++i){
			for(j = 0; j < h; ++j){
				if(lowCosts[i*h+j] > costs[(disparity-dMin)*h*w+i*h+j]){
					*disp.pixel(i,j) = static_cast<float>(disparity);
					lowCosts[i*h+j] = costs[(disparity-dMin)*h*w+i*h+j];
				}
			}	
		}
	}
}

int main (int argc, char** argv)
{
    if(argc != 6 && argc != 7) {
        std::cerr << "Usage: " << argv[0] << " imgIn1 imgIn2 dMin dMax"
                  << " dispMap [ssdMap]" << std::endl;
        return 1;
    }

    // Read input
    LWImage<float> im1(0,0,0), im2(0,0,0);
    if(! (loadImage(argv[1],im1) && loadImage(argv[2],im2)))
        return 1;

     // Read dispmin dispmax
    int dMin, dMax;
    std::istringstream f(argv[3]), g(argv[4]);
    if(! ((f>>dMin).eof() && (g>>dMax).eof())) {
        std::cerr << "Error reading dMin or dMax" << std::endl;
        return 1;
    }

    LWImage<float> disp = alloc_image<float>(im1.w,im1.h);
    LWImage<float>* ssdMap = 0;
    if(argc>6)
        ssdMap = new LWImage<float>(alloc_image<float>(im1.w,im1.h));
    disparity(im1, im2, dMin, dMax, win, disp, ssdMap);

	// these lines are here to transform float disparity into unsigned char disparity and save it into a png format
	unsigned char* disparity_map = new unsigned char[disp.w*disp.h];
	for(int i=0; i<disp.w*disp.h; ++i){
		disparity_map[i] = (unsigned char)(abs(10*disp.data[i]));
	}
	
	if(write_png_u8(argv[5], disparity_map, disp.w, disp.h, 1) != 0) {
        std::cerr << "Error writing file " << argv[5] << std::endl;
        return 1;
    }
    if(ssdMap&& write_tiff_f32(argv[6],ssdMap->data,ssdMap->w,ssdMap->h,1)!=0) {
        std::cerr << "Error writing file " << argv[6] << std::endl;
        return 1;
    }

    free(im1.data);
	free(im2.data);
	free(disp.data);
    if(ssdMap)
        free(ssdMap->data);
    delete ssdMap;

	std::cout << " disparity map computed " << std::endl;

    return 0;
}
