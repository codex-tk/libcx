/**
 */
#ifndef __cx_io_ip_basic_connector_h__
#define __cx_io_ip_basic_connector_h__


#include <cx/io/ip/basic_socket.hpp>
#include <cx/io/ip/option.hpp>
#include <cx/io/selector.hpp>

namespace cx::io::ip{

    template < int Type, int Proto >
    class basic_connector{
    public:
        using implementation = ip::socket_layer<Type, Proto>;
        basic_connector(void) {} 
        ~basic_connector(void) {}

        cx::io::ip::basic_socket< Type , Proto > connect( 
                const cx::io::ip::address& addr ,
                const std::chrono::milliseconds& ms )
        {
            cx::io::ip::basic_socket< Type , Proto > fd;
            if ( fd.open( addr.family() )) {
                fd.set_option( cx::io::ip::option::non_blocking());
                if ( implementation::connect( fd.handle() , addr )){
                    if ( cx::io::selector< socket_type , 1 >::select( fd.handle() 
                                , cx::io::ops::write 
                                , static_cast<int>(ms.count())) > 0 ) 
                    {
                        return fd;
                    }
                }
            }
            fd.close();
            return fd;
        }
    };

    namespace tcp {
        using connector = basic_connector< SOCK_STREAM , IPPROTO_TCP >;
    }
}

#endif