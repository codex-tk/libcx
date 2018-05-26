/**
 * @brief 
 * 
 * @file basic_socket.hpp
 * @author ghtak
 * @date 2018-05-22
 */

#ifndef __cx_io_ip_basic_socket_h__
#define __cx_io_ip_basic_socket_h__

#include <cx/io/ip/basic_address.hpp>

namespace cx::io::ip{

#if CX_PLATFORM == CX_P_WINDOWS
    using descriptor_type = SOCKET;
    static const descriptor_type invalid_descriptor = INVALID_SOCKET;
    static const int socket_error = SOCKET_ERROR;
#else
    using descriptor_type = int;
    static const descriptor_type invalid_descriptor = -1;
    static const descriptor_type socket_error = -1;
#endif   

    template < int Family , int Type , int Proto >
    class basic_socket {
    public:
        basic_socket( void ) : _fd(invalid_descriptor){}

        bool open( void ) {
#if CX_PLATFORM == CX_P_WINDOWS
            _fd = WSASocketW( Family , Type , Proto , nullptr , 0 , WSA_FLAG_OVERLAPPED  );
#else
            _fd = socket( Family , Type , Proto );   
#endif   
            return _fd != invalid_descriptor;
        }

        void close( void ) {
            if ( _fd != invalid_descriptor ) {
#if CX_PLATFORM == CX_P_WINDOWS
                closesocket( _fd );
#else
                close(_fd);
#endif      
            }
            _fd = invalid_descriptor;
        }

        bool bind( const ip::address& addr ) {
            return ::bind( _fd , addr.sockaddr() , addr.length()) != -1;
        }

        bool listen( void ) {
            return ::listen( _fd , SOMAXCONN ) != -1;
        }

        bool connect( const cx::io::ip::address& address ) {
            return ::connect( _fd , address.sockaddr() , address.length() );
        }

        descriptor_type descriptor( void ) {  
            return _fd;
        }

        template < typename T >
        bool set_option( T opt ) {
            return opt.set( _fd );
        }

        template < typename T >
        bool get_option( T& opt ) {
            return opt.get( _fd );
        }
    private:
        descriptor_type _fd;
    };

    namespace tcp {
        inline namespace v4 {
            using socket = basic_socket< AF_INET , SOCK_STREAM , IPPROTO_TCP >;
        }

        namespace v6{
            using socket = basic_socket< AF_INET6, SOCK_STREAM , IPPROTO_TCP >;
        }
    }
} // cx::io::ip

#endif