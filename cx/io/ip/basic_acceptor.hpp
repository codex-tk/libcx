/**
 * 
 * */

#ifndef __cx_io_ip_acceptor_h__
#define __cx_io_ip_acceptor_h__

#include <cx/io/ip/basic_socket.hpp>
#include <cx/io/ip/option.hpp>

namespace cx::io::ip {

    template < int Type , int Proto >
    class basic_acceptor {
    public:
        basic_acceptor( void ) {}
        ~basic_acceptor( void ) {}

        bool open( const uint16_t port , const uint16_t family ) {
            auto addresses = cx::io::ip::address::any( port , family );
            for ( auto addr : addresses ) {
                cx::io::ip::basic_socket<Type,Proto> fd;
                fd.open( addr.family());
                fd.set_option( cx::io::ip::option::reuse_address(cx::io::ip::option::enable));
                if ( addr.family() == AF_INET6 )
                    fd.set_option( cx::io::ip::option::bind_ipv6only(cx::io::ip::option::enable));
                fd.bind( addr );
                fd.listen();
                _listen_fds.push_back(fd);
            }
            return true;
        }
        void close(void){
            for ( auto fd : _listen_fds ) {
                fd.close();
            }
            _listen_fds.clear();
        }
    private:
        std::vector< cx::io::ip::basic_socket< Type , Proto > > _listen_fds;
        
    };

}

#endif