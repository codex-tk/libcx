/**
 * @brief 
 * 
 * @file socket_layer.hpp
 * @author ghtak
 * @date 2018-05-31
 */

#if CX_PLATFORM == CX_P_WINDOWS
#include <cx/io/ip/detail/win32_tcp_socket_layer.hpp>
namespace cx::io::ip::tcp {
    using socket_layer_impl = ip::detail::win32_tcp_socket_layer;
}
#else
#include <cx/io/ip/detail/posix_tcp_socket_layer.hpp>
namespace cx::io::ip::tcp {
    using socket_layer_impl = ip::detail::posix_tcp_socket_layer;
}
#endif