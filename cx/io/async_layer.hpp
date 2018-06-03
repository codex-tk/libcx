/**
 * @brief 
 * 
 * @file async_layer.hpp
 * @author ghtak
 * @date 2018-06-03
 */
#include <cx/io/async_layer_impl/iocp_async_layer.hpp>

namespace cx::io {

#if CX_PLATFORM == CX_P_WINDOWS
    using async_layer = detail::iocp_async_layer;
#elif CX_PLATFORM == CX_P_MACOSX
    using async_layer = detail::kqueue_async_layer;
#else 
    using async_layer = detail:epoll_async_layer;
#endif
    
}