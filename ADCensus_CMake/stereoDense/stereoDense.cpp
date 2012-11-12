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

static bool loadImage(const char* name, LWImage<float>& im) {
    size_t nx, ny;
    im.data = read_png_f32_gray(name, &nx, &ny);
    im.w = nx; im.h = ny;
    if(! im.data)
        std::cerr << "Unable to load image file " << name << std::endl;
    return (im.data!=0);
}

/// Compute disparity map from im1 to im2.
/// TODO with agregation & census windows, borders are more and more truncated....
static void disparity(LWImage<float> im1, LWImage<float> im2,
                      int dMin, int dMax, LWImage<float>& disp) 
{
	// variables
	int x, y, disparity, step, image;
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
	
	for(image = 1; image < 3; ++image){
		std::cout << "picture " << image << std::endl;
		params.activePicture = image;
		
		// computes ad census costs for each pixel and each disparity
		std::cout << "Phase 1: compute cost" << std::endl;
		for(x = 0; x < params.w; ++x){
			for(y = 0; y < params.h; ++y){
				for(disparity = dMin; disparity <= dMax; ++disparity){
					if(image == 1){
						if(x + disparity < params.winX || x + disparity >= w - params.winX || x < params.winX || x >= w-params.winX || y < params.winY || y > h-params.winY){
							costs[(disparity-dMin)*h*w+x*h+y] = 5;
						}
						else{
							costs[(disparity-dMin)*h*w+x*h+y] = adCensus(x, y, x+disparity, y, params);
						}
					}
					else{
						if(x - disparity < params.winX || x - disparity >= w - params.winX || x < params.winX || x >= w-params.winX || y < params.winY || y > h-params.winY){
							costs[(disparity-dMin)*h*w+x*h+y] = 5;
						}
						else{
							costs[(disparity-dMin)*h*w+x*h+y] = adCensus(x-disparity, y, x, y, params);
						}
					}
				}
			}
		}

		// agregate cost for each pichel in its adaptive window for each possible value of disparity
		std::cout << "Phase 2: agregate cost" << std::endl;
		for(disparity = dMin; disparity <= dMax; ++disparity){
			horizontalFirst = true;
			for(step = 0; step < params.nbAgregatingIteration; ++step){
				agregateCosts2D(costs, disparity, horizontalFirst, params);
				horizontalFirst = !horizontalFirst;
			}
		}
		
		// compute scanline optimization
		std::cout << "Phase 3: scanline optimization" << std::endl;
		costs = scanlineOptimization(costs, params);
		
		// find lowest cost and explicit disparity
		// initialize lowCosts table and disparity table
		std::cout << "Phase 4: compute best disparity" << std::endl;
		for(x = 0; x < w; ++x){
			for(y = 0; y < h; ++y){
				lowCosts[x*h+y] = costs[x*h+y];
				*disp.pixel(x+(image-1)*w,y) = static_cast<float>(dMin);
			}
		}

		// find best disparity
		for(disparity = dMin+1; disparity <= dMax; ++disparity){
			for(x = 0; x < w; ++x){
				for(y = 0; y < h; ++y){
					if(lowCosts[x*h+y] > costs[(disparity-dMin)*h*w+x*h+y]){
						*disp.pixel(x+(image-1)*w,y) = static_cast<float>(disparity);
						lowCosts[x*h+y] = costs[(disparity-dMin)*h*w+x*h+y];
					}
				}	
			}
		}
	}
}

int main (int argc, char** argv)
{
    if(argc != 6) {
        std::cerr << "Usage: " << argv[0] << " imgIn1 imgIn2 dMin dMax" << std::endl;
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

    LWImage<float> disp = alloc_image<float>(2*im1.w,im1.h);
    
	// compute disparity maps
	disparity(im1, im2, dMin, dMax, disp);
	std::cout << "*-----disparity map computed-----*" << std::endl;

	// these lines are here to transform float disparity into unsigned char disparity and save it into a png format
	unsigned char* disparity_map = new unsigned char[disp.w*disp.h];
	for(int i=0; i<disp.w*disp.h; ++i){
		disparity_map[i] = (unsigned char)(abs(10*disp.data[i]));
	}
	
	if(write_png_u8(argv[5], disparity_map, disp.w, disp.h, 1) != 0) {
        std::cerr << "Error writing file " << argv[5] << std::endl;
        return 1;
    }

    free(im1.data);
	free(im2.data);
	free(disp.data);

    return 0;
}
