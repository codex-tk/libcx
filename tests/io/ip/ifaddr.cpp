/**
 */

#include <cx/io/ip/ifaddr.hpp>

#include "tests/gprintf.hpp"

TEST( cx_io_ifaddr , ifaddrs ) {
    for( auto ifa : cx::io::ip::interfaces()) {
        gprintf( "%d %s %s %s %s" , 
                    if_nametoindex(ifa.name.c_str())
                    , ifa.name.c_str() 
                    , ifa.ip_address.c_str() 
                    , ifa.mask.c_str() 
                    , ifa.gateway.c_str() );
    }
}