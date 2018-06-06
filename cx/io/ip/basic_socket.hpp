/**
 * @brief 
 * 
 * @file basic_socket.hpp
 * @author ghtak
 * @date 2018-05-22
 */

#ifndef __cx_io_ip_basic_socket_h__
#define __cx_io_ip_basic_socket_h__

#include <cx/io/basic_object.hpp>

namespace cx::io::ip {

    template < typename ServiceType >
    class basic_socket : public cx::io::basic_object<ServiceType>{
    public:
        template < typename EngineType >
        basic_socket( EngineType& engine )
            : basic_object( engine )
        {
        }

        bool bind( const address_type& bind ) {
            return false;
            //return ::bind( _fd , addr.sockaddr() , addr.length()) != -1;
        }

        int shutdown( int how ) {
            return 0;
            //return ::shutdown( _fd , how );
        }

        address_type local_address( void ) const {
            /*
            cx::io::ip::address addr;
            ::getsockname( _fd , addr.sockaddr() , addr.length_ptr() );
            return addr;*/
            return address_type();
        }

        address_type remote_address( void ) const {
            /*
            cx::io::ip::address addr;
            ::getpeername( _fd , addr.sockaddr() , addr.length_ptr() );
            return addr;*/
            return address_type();
        }

        template < typename T >
        bool set_option( T opt ) {
            return false;
            //return opt.set( handle->set );
        }

        template < typename T >
        bool get_option( T& opt ) {
            return false;
            //return opt.get( _fd );
        }
    };

} // cx::io::ip

#endif
