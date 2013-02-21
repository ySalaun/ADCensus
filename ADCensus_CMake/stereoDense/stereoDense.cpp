#include "libStereo/patch.h"
#include "libStereo/scanlineOptimization.h"
#include "stereoDense/parameters.h"
#include "libIO/io_png.h"
#include "libIO/io_tiff.h"

#include <algorithm>
#include <iostream>
#include <sstream>

static bool loadImage(const char* name, LWImage<float>& im) {
    size_t nx, ny;
    im.data = read_png_f32_rgb(name, &nx, &ny);
    im.w = nx; im.h = ny;
    if(! im.data)
        std::cerr << "Unable to load image file " << name << std::endl;
    return (im.data!=0);
}

/// Compute disparity map from im1 to im2.
/// TODO with agregation & census windows, borders are more and more truncated....
static void disparity(LWImage<float> im1, LWImage<float> im2,
                      int dMin, int dMax, LWImage<float>& disp,
					  bool agregation_phase = true, bool so_phase = false) 
{
	
	PARAMETERS params(im1, im2, dMin, dMax);
	const int h = params.h, w = params.w;

	// tables
	float* costs = new float[h*w*(dMax-dMin+1)];
	float* lowCosts = new float[h*w];

	for(int image=0; image<=1; ++image) {
		std::cout << "picture " << image << std::endl;
		params.activePicture = image;

		// computes ad census costs for each pixel and each disparity
		std::cout << "Phase 1: compute cost" << std::endl;
#pragma omp parallel for
        for(int d=dMin; d<=dMax; ++d) {
            float* c=costs+(d-dMin)*w*h;
            for(int x=0; x<params.w; ++x){
                for(int y=0; y<params.h; ++y){
                    int x1=x, x2=x;
					if(image==0) x2=x+d; else x1=x-d;
                    bool out = (x1-params.winX<0 || x1+params.winX >= w ||
                                x2-params.winX<0 || x2+params.winX >= w ||
                                y -params.winY<0 || y +params.winY >= h);
                    *c++ = out? 5: adCensus(x1, y, x2, y, params);
				}
			}
        }

		// agregate cost for each pichel in its adaptive window for each possible value of disparity
		std::cout << "Phase 2: agregate cost" << std::endl;
		if(agregation_phase){
			for(int d=dMin; d<=dMax; ++d){
				bool horizontalFirst=true;
				for(int i=0; i<params.nbAgregatingIteration; i++){
					agregateCosts2D(costs+(d-dMin)*w*h, horizontalFirst, params);
					horizontalFirst = !horizontalFirst;
				}
			}
		}
		else{
			std::cout << "canceled" << std::endl;
		}

		// compute scanline optimization
		std::cout << "Phase 3: scanline optimization" << std::endl;
        if(so_phase){
			float* oldCosts=costs;
			costs = scanlineOptimization(costs, params);
			delete [] oldCosts;
		}
		else{
			std::cout << "canceled" << std::endl;
		}

		// find lowest cost and explicit disparity
		// initialize lowCosts table and disparity table
		std::cout << "Phase 4: compute best disparity" << std::endl;
		for(int x=0; x<w; ++x){
			for(int y=0; y<h; ++y){
				lowCosts[x*h+y] = costs[x*h+y];
				*disp.pixel(x+image*w,y) = static_cast<float>(dMin);
			}
		}

		// find best disparity
		for(int d=dMin+1; d<=dMax; ++d){
			for(int x=0; x<w; ++x){
				for(int y=0; y<h; ++y){
					if(lowCosts[x*h+y] > costs[(d-dMin)*h*w+x*h+y]){
						*disp.pixel(x+image*w,y) = static_cast<float>(d);
						lowCosts[x*h+y] = costs[(d-dMin)*h*w+x*h+y];
					}
				}
			}
		}
	}
    delete [] lowCosts;
    delete [] costs;
}

int main (int argc, char** argv)
{
    if(argc < 6 || argc > 8) {
		std::cerr << "Usage: " << argv[0] << " imgIn1 imgIn2 dMin dMax dispIm [agreg_on] [so_on]"
                  << std::endl;
        return 1;
    }

	bool agreg_on = (argc < 7 || atoi(argv[6]) != 0);
	bool so_on = (argc < 8 || atoi(argv[7]) != 0);

    // Read input
    LWImage<float> im1(0,0,0), im2(0,0,0);
    if(! (loadImage(argv[1],im1) && loadImage(argv[2],im2))){
        return 1;
	}

     // Read dispmin dispmax
    int dMin, dMax;
    std::istringstream f(argv[3]), g(argv[4]);
    if(! ((f>>dMin).eof() && (g>>dMax).eof())) {
        std::cerr << "Error reading dMin or dMax" << std::endl;
        return 1;
    }

    LWImage<float> disp = alloc_image<float>(2*im1.w,im1.h);

	// compute disparity maps
	disparity(im1, im2, dMin, dMax, disp, agreg_on, so_on);
	std::cout << "*-----disparity map computed-----*" << std::endl;

	// Transform float disparity into unsigned char and save into a PNG format
	unsigned char* disparity_map = new unsigned char[disp.w*disp.h];
	for(int i=0; i<disp.w*disp.h; ++i)
		disparity_map[i] = (unsigned char)(abs(10*disp.data[i]));

	if(write_png_u8(argv[5], disparity_map, disp.w, disp.h, 1) != 0) {
        std::cerr << "Error writing file " << argv[5] << std::endl;
        return 1;
    }

    delete [] disparity_map;
    free(im1.data);
	free(im2.data);
	free(disp.data);

    return 0;
}
