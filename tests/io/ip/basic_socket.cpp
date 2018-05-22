/**
 * @brief 
 * 
 * @file basic_socket.cpp
 * @author ghtak
 * @date 2018-05-22
 */
#include "gtest/gtest.h"
#include <cx/io/ip/basic_address.hpp>
#include <cx/io/ip/basic_socket.hpp>

TEST( cx_io_ip_socket , t0 ) { 
    cx::io::ip::tcp::socket fd;
    cx::io::ip::tcp::address addr;
    ASSERT_NE(  addr.address() , nullptr );
    
    ASSERT_TRUE( fd.open() );
    ASSERT_NE( fd.descriptor() 
            , cx::io::ip::tcp::socket::invalid_descriptor );
    fd.close();
}