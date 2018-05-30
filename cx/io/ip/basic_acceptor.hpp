/**
 * 
 * */

#ifndef __cx_io_ip_acceptor_h__
#define __cx_io_ip_acceptor_h__

#include <cx/io/ip/basic_socket.hpp>
#include <cx/io/ip/option.hpp>
#include <cx/io/selector.hpp>

namespace cx::io::ip {

    template < int Type , int Proto , typename Impl >
    class basic_acceptor {
    public:
        basic_acceptor( void ) {}
        ~basic_acceptor( void ) {}

        bool open( const uint16_t port , const uint16_t family ) {
            auto addresses = cx::io::ip::address::any( port , family );
            for ( auto addr : addresses ) {
                cx::io::ip::basic_socket<Type,Proto,Impl> fd;
                if ( fd.open( addr.family())) {
                    fd.set_option( cx::io::ip::option::reuse_address(cx::io::ip::option::enable));
                    if ( addr.family() == AF_INET6 )
                        fd.set_option( cx::io::ip::option::bind_ipv6only(cx::io::ip::option::enable));
                    if ( fd.bind( addr ) && fd.listen() ){
                        _listen_fds.emplace_back(fd.descriptor(cx::io::invalid_descriptor));
                    }
                }
                fd.close();
            }
            for ( auto fd : _listen_fds ) {
                _selector.bind( fd.descriptor(), cx::io::ops::read , nullptr );
            }
            return !_listen_fds.empty();
        }
        void close(void){
            for ( auto fd : _listen_fds ) {
                _selector.unbind( fd.descriptor() );
                fd.close();
            }
            _listen_fds.clear();
        }

        template < typename Handler >
        void handle_accept( const Handler& handler  , const std::chrono::milliseconds& ms ){
            
        }
    private:
        std::vector< cx::io::ip::basic_socket< Type , Proto , Impl > > _listen_fds;
        cx::io::selector _selector;
    };

    namespace tcp {
        using acceptor = basic_acceptor<  SOCK_STREAM , IPPROTO_TCP , tcp::socket_layer_impl >;
    }


}

#endif