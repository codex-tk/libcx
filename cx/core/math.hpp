#ifndef __cx_core_math_h__
#define __cx_core_math_h__

#include <cx/cxdefine.hpp>

namespace cx::core {
    
    double radian( double degree ) {
        return degree * M_PI / 180;
    }

    double degree( double radian ) {
        return radian * 180 / M_PI;
    }
}

#endif