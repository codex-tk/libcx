#ifndef __cx_vision_harris_h__
#define __cx_vision_harris_h__

#include <cx/vision/vision.hpp>
#include <cx/vision/image_proc.hpp>

namespace cx { namespace vision {

void harris_corner( const image& src
  , double corner_threshold // 1e8 ( 10^8 )
  , double k  // 0.04
  , std::vector< cx::vision::point >& points );

}}

#endif
