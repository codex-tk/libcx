#ifndef __cx_vision_fft_h__
#define __cx_vision_fft_h__

#include <cx/vision/image.hpp>

namespace cx { namespace vision {

void fft1d( double* re , double* im , int N , int isign );

int  fft_size( int n );

void fft_mag_image( const image_base<double>& re
                    , const image_base<double>& im
                    , image_base<uint8_t>& dst );

void fft_phs_image( const image_base<double>& re
                    , const image_base<double>& im
                    , image_base<uint8_t>& dst );

void fft_shift( image_base<double>& img );

void fft( const image_base<uint8_t>& gray_image
  , image_base<double>& fft_re
  , image_base<double>& fft_im );

}}


#endif // __cx_vision_fft_h__
