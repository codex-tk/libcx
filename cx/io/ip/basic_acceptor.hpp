/**
 * 
 * */

#ifndef __cx_io_ip_acceptor_h__
#define __cx_io_ip_acceptor_h__

#include <cx/io/ip/basic_socket.hpp>

namespace cx::io::ip {

    template < int Type , int Proto >
    class basic_acceptor {
    public:
        basic_acceptor( void ) {}
        ~basic_acceptor( void ) {}

        bool open( const uint16_t family , const uint16_t port ) {
            return false;
        }
    private:
        cx::io::ip::basic_socket< Type , Proto > _fd;
        
    };

}

#endif