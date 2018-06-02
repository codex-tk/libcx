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
#if CX_PLATFORM == CX_P_WINDOWS        
using selector_t = cx::io::selector< SOCKET , 32 >;
#else
using selector_t = cx::io::selector< int , 32 >;
#endif


TEST( cx_io_ip_socket , open_close ) { 
    ip::tcp::socket fd;
    ip::address addr;
    ASSERT_NE( addr.sockaddr() , nullptr );
    
    ASSERT_TRUE( fd.open() );
    ASSERT_NE( fd.handle() 
            , cx::io::ip::invalid_socket );
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

    ASSERT_TRUE( ip::tcp::socket::implementation::connect( fd.handle() , addresses[0]) );
    ASSERT_TRUE( selector_t::select( fd.handle() , io::ops::write , 5000 ) == io::ops::write );
    ASSERT_EQ( fd.write(  io::basic_buffer( "GET / HTTP/1.1\r\n\r\n" ) ) 
    , strlen("GET / HTTP/1.1\r\n\r\n"));
    ASSERT_TRUE( selector_t::select( fd.handle() , io::ops::read , 5000 ) == io::ops::read );
    char buf[4096] = { 0 , };
    int len = fd.read( io::basic_buffer( buf , 1024 ) );
    ASSERT_TRUE( len > 0 );
    gprintf( "Local : %s" , fd.local_address().to_string().c_str() );
    gprintf( "Remote : %s" , fd.remote_address().to_string().c_str());
#if CX_PLATFORM == CX_P_WINDOWS
    fd.shutdown(SD_BOTH);
#else
    fd.shutdown(SHUT_RDWR);
#endif
    fd.close();
}

TEST( cx_io_ip , any ) {
    auto ipv4 = cx::io::ip::address::any( 80 , AF_INET );
    auto ipv6 = cx::io::ip::address::any( 80 , AF_INET6 );
    auto all = cx::io::ip::address::any( 80 , AF_UNSPEC );
    std::for_each( ipv4.begin() , ipv4.end() , [] ( auto a ) { gprintf( "V4 %s" , a.to_string().c_str());} );
    std::for_each( ipv6.begin() , ipv6.end() , [] ( auto a ) { gprintf( "V6 %s" ,a.to_string().c_str());} );
    std::for_each( all.begin() , all.end() , [] ( auto a ) { gprintf( "ALL %s" ,a.to_string().c_str());} );
}

TEST( cx_io_ip_socket , sample_echo ) {
    char buf[1024] = { 0 , };

    ip::tcp::socket server;
    ASSERT_TRUE( server.open() );
    ASSERT_TRUE( server.set_option( ip::option::reuse_address(ip::option::enable)));
    ASSERT_TRUE( server.set_option( ip::option::non_blocking()));
    ASSERT_TRUE( server.bind(ip::address::any(7543 , AF_INET )[0]));
    ASSERT_TRUE( ip::tcp::socket::implementation::listen( server.handle() ) );

    ip::tcp::socket client;
    ip::address clientaddr( AF_INET , "127.0.0.1" , 7543 );
    ASSERT_TRUE( client.open() );
    ASSERT_TRUE( ip::tcp::socket::implementation::connect( client.handle() , clientaddr) );
    ASSERT_TRUE( selector_t::select( client.handle() , io::ops::write , 5000 ) == io::ops::write );
    
    ASSERT_TRUE( selector_t::select( server.handle() , io::ops::read , 1000 ) == io::ops::read );

    ip::address addr;
    ip::tcp::socket accepted( ip::tcp::socket::implementation::accept(server.handle() , addr ));   
    ASSERT_TRUE( accepted.handle() != io::ip::invalid_socket );

    ASSERT_EQ( client.write(io::basic_buffer("Hello")) , strlen("Hello"));
    ASSERT_EQ( accepted.read( io::basic_buffer(buf,1024)), strlen("Hello"));
    ASSERT_STREQ( "Hello" , buf );

    accepted.write(io::basic_buffer("World"));
    ASSERT_EQ( client.read( io::basic_buffer(buf,1024)), strlen("World"));
    ASSERT_STREQ( "World" , buf );

    server.close();
    accepted.close();
    client.close();
}
