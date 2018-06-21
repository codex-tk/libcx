#ifndef __cx_vision_canny_h__
#define __cx_vision_canny_h__

#include <cx/vision/vision.hpp>
#include <cx/vision/image_proc.hpp>

namespace cx { namespace vision {

void canny_edge( const image& src , image& dst , uint8_t threshold_hi , uint8_t threashold_lo );

}}

#endif
