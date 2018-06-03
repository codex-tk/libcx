/**
 * @brief 
 * 
 * @file basic_socket.hpp
 * @author ghtak
 * @date 2018-05-22
 */

#ifndef __cx_io_ip_basic_socket_h__
#define __cx_io_ip_basic_socket_h__

#include <cx/io/ip/socket_layer.hpp>

namespace cx::io::ip {

    template < int Type , int Proto >
    class basic_socket {
    public:
        using implementation = ip::socket_layer<Type,Proto>;
        using buf_type = typename implementation::buf_type;
        using bufv_type = typename implementation::bufv_type;

        basic_socket( void ) : _fd(invalid_socket){}
        basic_socket( socket_type fd ) : _fd(fd){}

        bool open( int family = AF_INET ) {
            _fd = implementation::open( family );
            return _fd != invalid_socket;
        }

        void close( void ) {
            if ( _fd != invalid_socket ) {
                implementation::close(_fd);
            }
            _fd = invalid_socket;
        }

        bool bind( const cx::io::ip::address& addr ) {
            return ::bind( _fd , addr.sockaddr() , addr.length()) != -1;
        }

        int write( const buf_type& buf ) {
            return implementation::write(_fd,buf);
        }

        int read( const buf_type& buf ) {
            return implementation::read(_fd,buf);
        }

        int writev( const bufv_type& buf ) {
            return implementation::writev(_fd,buf);
        }

        int readv( const bufv_type& buf ) {
            return implementation::readv(_fd,buf);
        }

        int shutdown( int how ) {
            return ::shutdown( _fd , how );
        }

        cx::io::ip::address local_address( void ) const {
            cx::io::ip::address addr;
            ::getsockname( _fd , addr.sockaddr() , addr.length_ptr() );
            return addr;
        }

        cx::io::ip::address remote_address( void ) const {
            cx::io::ip::address addr;
            ::getpeername( _fd , addr.sockaddr() , addr.length_ptr() );
            return addr;
        }

        socket_type handle( void ) {  
            return _fd;
        }

        socket_type handle( socket_type fd ) {
            std::swap( _fd , fd );
            return fd;
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
        socket_type _fd;
    };

    namespace tcp {
        using socket = basic_socket<  SOCK_STREAM , IPPROTO_TCP >;
    }

    namespace udp {
        //using socket = basic_socket<  SOCK_DGRAM , IPPROTO_UDP >;
    }
} // cx::io::ip

#endif
