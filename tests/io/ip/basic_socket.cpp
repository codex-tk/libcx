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
    ip::tcp::socket fd;
    
    auto addresses = ip::address::resolve( "naver.com" , 80 );
    for ( auto a : addresses ) {
        gprintf( a.to_string().c_str() );
    }

    ASSERT_FALSE( addresses.empty());
    ASSERT_TRUE( fd.open() );
    //ip::address addr( AF_INET , "216.58.197.206" , 80 );
    //ASSERT_TRUE( fd.connect(addr) );

    ASSERT_TRUE( fd.connect(addresses[0]) );
    ASSERT_TRUE( io::selector::select( fd , io::ops::write , 5000 ) == io::ops::write );
    ASSERT_EQ( fd.write( "GET / HTTP/1.1\r\n\r\n" ) , strlen("GET / HTTP/1.1\r\n\r\n"));
    ASSERT_TRUE( io::selector::select( fd , io::ops::read , 5000 ) == io::ops::read );
    char buf[4096] = { 0 , };
    int len = fd.read( io::buffer( buf , 1024 ) );
    ASSERT_TRUE( len > 0 );
    gprintf( "Local : %s" , fd.local_address().to_string().c_str() );
    gprintf( "Remote : %s" , fd.remote_address().to_string().c_str());
    fd.shutdown(SD_BOTH);
    fd.close();
}

TEST( cx_io_ip_socket , sample_echo ) {
    char buf[1024] = { 0 , };

    ip::tcp::socket server;
    ASSERT_TRUE( server.open() );
    ASSERT_TRUE( server.set_option( ip::option::reuse_address(ip::option::enable)));
    ASSERT_TRUE( server.set_option( ip::option::non_blocking()));
    ASSERT_TRUE( server.bind(ip::address::any(7543)));
    ASSERT_TRUE( server.listen() );

    ip::tcp::socket client;
    ip::address clientaddr( AF_INET , "127.0.0.1" , 7543 );
    
    ASSERT_TRUE( client.open() );
    ASSERT_TRUE( client.connect( clientaddr ));
    ASSERT_TRUE( io::selector::select( client , io::ops::write , 5000 ) == io::ops::write );
    
    ASSERT_TRUE( io::selector::select( server , io::ops::read , 1000 ) == io::ops::read );

    ip::address addr;
    ip::tcp::socket accepted( server.accept( addr ));   
    ASSERT_TRUE( accepted.descriptor() != ip::socket_error );

    ASSERT_EQ( client.write("Hello") , strlen("Hello"));
    ASSERT_EQ( accepted.read( io::buffer(buf,1024)), strlen("Hello"));
    ASSERT_STREQ( "Hello" , buf );

    accepted.write("World");
    ASSERT_EQ( client.read( io::buffer(buf,1024)), strlen("World"));
    ASSERT_STREQ( "World" , buf );

    server.close();
    accepted.close();
    client.close();
}