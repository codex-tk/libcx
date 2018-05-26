/**
 * @brief 
 * 
 * @file basic_socket.cpp
 * @author ghtak
 * @date 2018-05-22
 */
#include "gtest/gtest.h"
#include <cx/io/ip/basic_socket.hpp>
#include <cx/io/ip/option.hpp>

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
    ip::address::resolve(u8"google.com");
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