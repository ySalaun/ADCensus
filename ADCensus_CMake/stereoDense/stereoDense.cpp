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
static void disparity(LWImage<float> im1, LWImage<float> im2,
                      int dMin, int dMax, int win,
                      LWImage<float>& disp, LWImage<float>* costMap) {
    float (*dispFunction)(const LWImage<float>&,int,int,
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
		disparity_map[i] = (unsigned char)(10*disp.data[i]);
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
