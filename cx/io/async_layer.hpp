/**
 * @brief 
 * 
 * @file async_layer.hpp
 * @author ghtak
 * @date 2018-06-03
 */
#include <cx/io/detail/win32_async_layer.hpp>

namespace cx::io {

#if CX_PLATFORM == CX_P_WINDOWS
    using async_layer = detail::win32_async_layer;
#else

#endif
    
}