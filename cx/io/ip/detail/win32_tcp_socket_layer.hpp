/**
 * @brief 
 * 
 * @file win32_tcp_socket_layer.hpp
 * @author ghtak
 * @date 2018-05-31
 */
#include <cx/io/ip/basic_address.hpp>

#if CX_PLATFORM == CX_P_WINDOWS
namespace cx::io::ip::detail {
    class win32_tcp_socket_layer {
    public:
        static cx::io::descriptor_type open( int family , int type , int proto ) {
            return ::WSASocketW( family , type , proto , nullptr , 0 , WSA_FLAG_OVERLAPPED  ); 
        }
        static void close( cx::io::descriptor_type fd ) {
            ::closesocket( fd );
        }
    };
}
#endif