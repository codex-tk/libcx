
#include "gtest/gtest.h"
#include "tests/gprintf.hpp"
#include <cx/io/services.hpp>

TEST( cx_io_ip , any ) {
    auto ipv4 = cx::io::ip::tcp::address::anys( 80 , AF_INET );
    auto ipv6 = cx::io::ip::tcp::address::anys( 80 , AF_INET6 );
    auto all  = cx::io::ip::tcp::address::anys( 80 , AF_UNSPEC );
    std::for_each( ipv4.begin() , ipv4.end() , [] ( auto a ) { gprintf( "V4 %s" , a.to_string().c_str());} );
    std::for_each( ipv6.begin() , ipv6.end() , [] ( auto a ) { gprintf( "V6 %s" ,a.to_string().c_str());} );
    std::for_each( all.begin() , all.end() , [] ( auto a ) { gprintf( "ALL %s" ,a.to_string().c_str());} );
}