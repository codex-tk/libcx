/**
 * @brief 
 * 
 * @file basic_socket.cpp
 * @author ghtak
 * @date 2018-05-22
 */
#include "gtest/gtest.h"
#include "tests/gprintf.hpp"
#include <cx/io/ip/basic_socket.hpp>
#include <cx/io/ip/option.hpp>
#include <cx/io/selector.hpp>

using namespace cx;
using namespace io;

TEST( cx_io_ip_socket , open_close ) { 
    ip::tcp::socket fd;
    ip::address addr;
    ASSERT_NE( addr.sockaddr() , nullptr );
    
    ASSERT_TRUE( fd.open() );
    ASSERT_NE( fd.descriptor() 
            , ip::invalid_descriptor );
    fd.close();
}

TEST( cx_io_ip_socket , connect ) {
    auto addresses = ip::address::resolve( "google.com" , 80);
    for ( auto a : addresses ) {
        char ipaddress[1024];
        a.inet_ntop( ipaddress , 1024 );
        gprintf( ipaddress );
        gprintf( a.to_string().c_str() );
    }
    ASSERT_FALSE( addresses.empty());
    ip::tcp::socket fd;
    ASSERT_TRUE( fd.open() );
    //ASSERT_TRUE( fd.set_option( ip::option::non_blocking()) );
    ASSERT_TRUE( fd.connect(addresses[0]) );
    ASSERT_TRUE( io::selector::select( fd , io::ops::write , 5000 ) == io::ops::write );
    
    ASSERT_EQ( fd.write( "GET / HTTP/1.1\r\n\r\n" ) , strlen("GET / HTTP/1.1\r\n\r\n"));
}

TEST( cx_io_ip_socket , bind ) {
    ip::tcp::socket fd;
    ASSERT_TRUE( fd.open() );
    ASSERT_TRUE( fd.set_option( ip::option::reuse_address(true)));
    ASSERT_TRUE( fd.set_option( ip::option::non_blocking()));
    ASSERT_TRUE( fd.bind(ip::address::any(7543)));
    ASSERT_TRUE( fd.listen() );
    fd.close();
}