#include "libStereo/patch.h"
#include "libStereo/scanlineOptimization.h"
#include "stereoDense/parameters.h"
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
	int x, y, disparity, step;
	bool horizontalFirst;

	// parameters initialization
	PARAMETERS params;
	params.initialization(im1, im2, dMin, dMax);
	int h = params.h, w = params.w;
	
	// tables
	float *costs;
	float *lowCosts;
	costs = new float[h*w*(dMax-dMin+1)];
	lowCosts = new float[h*w];
	
	// computes ad census costs for each pixel and each disparity
	for(x = 0; x < params.w; ++x){
		for(y = 0; y < params.h; ++y){
            for(disparity = dMin; disparity <= dMax; ++disparity){
				if(x + disparity < win || x + disparity >= w - win || x < win || x >= w-win || y < win || y > h-win){
					costs[(disparity-dMin)*h*w+x*h+y] = 5;
				}
				else{
					costs[(disparity-dMin)*h*w+x*h+y] = adCensus(x, y, x+disparity, y, params);
				}
			}
		}
	}
	std::cout << "test" << std::endl;
	// agregate cost for each pichel in its adaptive window for each possible value of disparity
	for(disparity = dMin; disparity <= dMax; ++disparity){
		horizontalFirst = true;
		for(step = 0; step < params.nbAgregatingIteration; ++step){
			agregateCosts2D(costs, disparity, horizontalFirst, params);
			horizontalFirst = !horizontalFirst;
		}
	}
	std::cout << "test" << std::endl;
	// compute scanline optimization
	costs = scanlineOptimization(costs, params);

	// find lowest cost and explicit disparity
	// initialize lowCosts table and disparity table
	for(x = 0; x < w; ++x){
		for(y = 0; y < h; ++y){
			lowCosts[x*h+y] = costs[x*h+y];
			*disp.pixel(x,y) = static_cast<float>(dMin);
		}
	}
	// find best disparity
	for(disparity = dMin+1; disparity <= dMax; ++disparity){
		for(x = 0; x < w; ++x){
			for(y = 0; y < h; ++y){
				if(lowCosts[x*h+y] > costs[(disparity-dMin)*h*w+x*h+y]){
					*disp.pixel(x,y) = static_cast<float>(disparity);
					lowCosts[x*h+y] = costs[(disparity-dMin)*h*w+x*h+y];
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
