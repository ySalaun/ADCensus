#include "libStereo/patch.h"
#include "libStereo/scanlineOptimization.h"
#include "libStereo/occlusion.h"
#include "stereoDense/parameters.h"
#include "libIO/io_png.h"
#include "libIO/cmdLine.h"

#include <algorithm>
#include <limits>
#include <iostream>
#include <sstream>

/// Names of occlusion output image files
static const char* OUTFILE2="disparity_occlusion.png";
static const char* OUTFILE3="disparity_occlusion_filled.png";
static const char* OUTFILE4="disparity_occlusion_filled_smoothed.png";

static bool loadImage(const char* name, LWImage<float>& im) {
    size_t nx, ny;
    im.data = io_png_read_f32_rgb(name, &nx, &ny);
    im.w = nx; im.h = ny;
    if(! im.data)
        std::cerr << "Unable to load image file " << name << std::endl;
    return (im.data!=0);
}

/// Save \a disparity image in 8-bit PNG image.
///
/// The disp->gray function is affine: gray=a*disp+b.
/// Pixels outside [0,255] are assumed invalid and written in cyan color.
bool save_disparity(const char* fileName, const LWImage<int>& disparity,
                    int dMin, int dMax, int grayMin, int grayMax)
{
    const float a=(grayMax-grayMin)/float(dMax-dMin);
    const float b=(grayMin*dMax-grayMax*dMin)/float(dMax-dMin);

    const int w=disparity.w, h=disparity.h;
    const int* in=disparity.data;
    unsigned char *out = new unsigned char[3*w*h];
    unsigned char *red=out, *green=out+w*h, *blue=out+2*w*h;
    for(size_t i=w*h; i>0; i--, in++, red++) {
        if(dMin<=*in && *in<=dMax) {
            float v = a * *in + b +0.5f;
            if(v<0) v=0;
            if(v>255) v=255;
            *red = static_cast<unsigned char>(v);
            *green++ = *red;
            *blue++  = *red;
        } else { // Cyan for disparities out of range
            *red=0;
            *green++ = *blue++ = 255;
        }
    }
    bool ok = (io_png_write_u8(fileName, out, w, h, 3) == 0);
    delete [] out;
    return ok;
}

/// Compute disparity map from im1 to im2.
/// TODO with agregation & census windows, borders are more and more truncated....
static void disparity(LWImage<float> im1, LWImage<float> im2,
                      int dMin, int dMax,
                      LWImage<int>& disp1, LWImage<int>& disp2) {
	PARAMETERS params(im1, im2, dMin, dMax);
	const int h = params.h, w = params.w;

	// tables
	float* costVol = new float[h*w*(dMax-dMin+1)];
	float* lowCosts = new float[h*w];

	for(int image=0; image<=1; ++image) {
		std::cout << "picture " << image << std::endl;
		params.activePicture = image;

		// computes ad census costs for each pixel and each disparity
		std::cout << "Phase 1: compute cost" << std::endl;
#pragma omp parallel for
        for(int d=dMin; d<=dMax; ++d) {
            float* c=costVol+(d-dMin)*w*h;
            for(int x=0; x<params.w; ++x)
                for(int y=0; y<params.h; ++y){
                    int x1=x, x2=x;
					if(image==0) x2=x+d; else x1=x-d;
                    bool out = (x1-params.winX<0 || x1+params.winX >= w ||
                                x2-params.winX<0 || x2+params.winX >= w ||
                                y -params.winY<0 || y +params.winY >= h);
                    *c++ = out? 5: adCensus(x1, y, x2, y, params);
				}
        }

		// agregate cost for each pichel in its adaptive window for each possible value of disparity
		std::cout << "Phase 2: agregate cost" << std::endl;
		for(int d=dMin; d<=dMax; ++d){
			bool horizontalFirst=true;
			for(int i=0; i<params.nbAgregatingIteration; i++){
				agregateCosts2D(costVol+(d-dMin)*w*h, horizontalFirst, params);
				horizontalFirst = !horizontalFirst;
			}
		}

		// compute scanline optimization
		std::cout << "Phase 3: scanline optimization" << std::endl;
        float* oldCosts=costVol;
		costVol = scanlineOptimization(costVol, params);
        delete [] oldCosts;

		// find lowest cost and explicit disparity
		std::cout << "Phase 4: compute best disparity" << std::endl;
        LWImage<int>& disp = (image==0)? disp1: disp2;
        std::fill_n(lowCosts, w*h, std::numeric_limits<float>::max());
        std::fill_n(disp.data, disp.w*disp.h, dMin);
		for(int d=dMin; d<=dMax; ++d)
			for(int x=0; x<w; ++x)
				for(int y=0; y<h; ++y)
					if( lowCosts[x*h+y] > costVol[(d-dMin)*h*w+x*h+y]) {
						lowCosts[x*h+y] = costVol[(d-dMin)*h*w+x*h+y];
						*disp.pixel(x,y) = d;
					}
	}
    delete [] lowCosts;
    delete [] costVol;
}

int main (int argc, char** argv)
{
    int grayMin=255, grayMax=0;
    char sense='r'; // Camera motion direction: 'r'=to-right, 'l'=to-left
    CmdLine cmd;

    ParamOcclusion paramOcc; // Parameters for filling occlusions
    cmd.add( make_option('o',paramOcc.tol_disp) ); // Detect occlusion
    cmd.add( make_option('O',sense) ); // Fill occlusion
    cmd.add( make_option('r',paramOcc.median_radius) );
    cmd.add( make_option('c',paramOcc.sigma_color) );
    cmd.add( make_option('s',paramOcc.sigma_space) );

    cmd.add( make_option('a',grayMin) );
    cmd.add( make_option('b',grayMax) );
    try {
        cmd.process(argc, argv);
    } catch(std::string str) {
        std::cerr << "Error: " << str << std::endl<<std::endl;
        return 1;
    }
    if(argc != 6) {
        ParamOcclusion q;
        std::cerr << "Usage: " << argv[0]
                  << " [options] im1.png img2.png dMin dMax disp.png"
                  << "Occlusion detection:\n"
                  << "    -o tolDiffDisp: tolerance for LR disp. diff. ("
                  <<q.tol_disp << ")\n\n"
                  << "Densification:\n"
                  << "    -O sense: fill occlusion, sense='r':right, 'l':left\n"
                  << "    -r radius: radius of the weighted median filter ("
                  <<q.median_radius << ")\n"
                  << "    -c sigmac: value of sigma_color ("
                  <<q.sigma_color << ")\n"
                  << "    -s sigmas: value of sigma_space ("
                  <<q.sigma_space << ")\n\n"
                  << "    -a grayMin: value of gray for min disparity (255)\n"
                  << "    -b grayMax: value of gray for max disparity (0)"
                  << std::endl;
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

    if(sense != 'r' && sense != 'l') {
        std::cerr << "Error: invalid camera motion direction " << sense
                  << " (must be r or l)" << std::endl;
        return 1;
    }

    LWImage<int> disp1 = alloc_image<int>(im1.w,im1.h);
    LWImage<int> disp2 = alloc_image<int>(im2.w,im2.h);

	// compute disparity maps
	disparity(im1, im2, dMin, dMax, disp1, disp2);
	std::cout << "*-----disparity map computed-----*" << std::endl;

	if(! save_disparity(argv[5], disp1, dMin,dMax, grayMin,grayMax)) {
        std::cerr << "Error writing file " << argv[5] << std::endl;
        return 1;
    }

    if(cmd.used('o') || cmd.used('O')) { // Occlusion detection
        detect_occlusion(disp1, disp2, dMin-1, paramOcc.tol_disp);
        if(! save_disparity(OUTFILE2, disp1, dMin,dMax, grayMin,grayMax))  {
            std::cerr << "Error writing file " << OUTFILE2 << std::endl;
            return 1;
        }
    }

    if(cmd.used('O')) { // Occlusion filling
        LWImage<int> dispDense = alloc_image<int>(disp1.w,disp1.h);
        std::copy(disp1.data, disp1.data+disp1.w*disp1.h, dispDense.data);
        if(sense == 'r')
            fillMaxX(dispDense, dMin);
        else
            fillMinX(dispDense, dMin);
        if(! save_disparity(OUTFILE3, dispDense, dMin,dMax, grayMin,grayMax)) {
            std::cerr << "Error writing file " << OUTFILE3 << std::endl;
            return 1;
        }

        std::cout << "Post-processing: smooth the disparity map" << std::endl;
        fill_occlusion(dispDense, im1,
                       disp1, dMin, dMax, paramOcc);
        if(! save_disparity(OUTFILE4, disp1, dMin,dMax, grayMin,grayMax)) {
            std::cerr << "Error writing file " << OUTFILE4 << std::endl;
            return 1;
        }
    }

    free(im1.data);
	free(im2.data);
	free(disp1.data);
	free(disp2.data);

    return 0;
}
