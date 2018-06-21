#ifndef __cx_vision_image_draw_h__
#define __cx_vision_image_draw_h__

#include <cx/vision/image.hpp>

namespace cx { namespace vision {

    void line_to( image& img
                  , const cx::vision::point& from
                  , const cx::vision::point& to
                  , const uint8_t color
                  );

}}

#endif
