/* TIFF float image conversion to color PNG for visualization.
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

#include "libIO/io_tiff.h"
#include "libIO/io_png.h"
#include "libIO/nan.h"

#include <algorithm>
#include <sstream>
#include <iostream>
#include <cmath>
#include <cstdlib>

/// Put to true to have transparency layer
static const bool bTransparent=false;
/// Background color to put to invalid pixels
static const unsigned char bgColor[]={0,255,255};
/// Percent of extreme values to ignore in min/max computation
static const float percent_extreme = 1.0f;

// Find min/max
static void find_minmax(const float* im, size_t n, float& a, float& b)
{
    float *v=new float[n], *end=v, *p;
    for(size_t i=0; i<n; i++)
        if( is_number(im[i]) )
            *end++ = im[i];
    n = end-v;
    const size_t skip = static_cast<size_t>(floor(n * 0.01f*::percent_extreme));
    p = v+skip;
    std::nth_element(v, p, end); a = *p;
    p = end-1-skip;
    std::nth_element(v, p, end); b = *p;
    delete [] v;
}

// Is there NaN in the image?
static bool has_nan(const float* im, size_t n)
{
    for(; n-- > 0; im++)
        if(*im != *im)
            return true;
    return false;
}

/// Usage: convert im_float.tif im.png [min max]
/// Convert the TIFF float image @im_float.tif into PNG 8-bit image @im.png. If
/// @min and @max are given (float values), they are to be mapped to 0 and 255
/// respectively, with linear interpolation in-between. NaN pixels are mapped
/// to a cyan value in the output image.
int main(int argc, char** argv)
{
    if(argc != 3 && argc != 5) {
        std::cerr << "Usage: " << argv[0] << " im_float.tif im.png [min max]" <<std::endl;
        return 1;
    }

    size_t w=0, h=0;
    float* im = read_tiff_f32_gray(argv[1], &w, &h);
    if(! im) {
        std::cerr << "Impossible to read float image " << argv[1] <<std::endl;
        return 1;
    }

    float a, b;
    if(argc > 4) {
        if(! (std::stringstream(argv[3]) >> a).eof()) {
            std::cerr << "Unable to interpret " << argv[3]
                      << " as min value"<<std::endl;
            return 1;
        }
        if(! (std::stringstream(argv[4]) >> b).eof()) {
            std::cerr << "Unable to interpret " << argv[4]
                      << " as max value"<<std::endl;
            return 1;
        }
    } else {
        find_minmax(im, w*h, a, b);
        std::cout << "min/max: " << a << " " << b << std::endl;
    }

    // Linear map from [a,b] to [256,0]: 256*(b-x)/(b-a)=(b-x)*(256/(b-a))
    if(b <= a) {
        a = -128.0f/a;
        b = 0;
    } else
        a = 256.0f / (b-a);

    const float* in = im;
    const size_t n = (bTransparent? 2: 3) * w*h;
    unsigned char* out = new unsigned char[n];
    unsigned char *red=out, *green=out+w*h, *blue=out+2*w*h;
    for(size_t i=w*h; i>0; i--, in++, red++) {
        float v = (b-*in)*a;
        if(v != v) v = bgColor[0];
        else if(v < 0.0f) v = 0.0f;
        else if(v > 255.0f) v = 255.0f;
        *red = static_cast<unsigned char>(v);
        if(bTransparent)
            *green++ = (*in==*in)? 255: 0;
        else {
            *green++ = (*in==*in)? *red: bgColor[1];
            *blue++  = (*in==*in)? *red: bgColor[2];
        }
    }

    bool bNaN = has_nan(im, w*h);
    const size_t channels = (bNaN? (bTransparent? 2: 3): 1);
    if(write_png_u8(argv[2], out, w, h, channels) != 0) {
        std::cerr << "Impossible to write png image " << argv[2] <<std::endl;
        return 1;
    }

    free(im);
    delete [] out;
    return 0;
}
