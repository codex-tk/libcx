
#include "gtest/gtest.h"
#include "tests/gprintf.hpp"
#include <cx/io/services.hpp>

TEST( cx_io_ip_sockets , t0 ) {
    cx::io::basic_engine< 
        cx::io::basic_implementation
        , cx::io::ip::tcp::service  
        , cx::io::ip::udp::service > engine;
    cx::io::ip::tcp::socket fd(engine);
    cx::io::ip::tcp::address addr(AF_INET , "127.0.0.1" , 7543);
    gprintf( "%s" , addr.to_string().c_str());

    auto addresses = cx::io::ip::tcp::address::resolve( "naver.com" , 80 , AF_INET );
    for ( auto a : addresses ) {
        gprintf( a.to_string().c_str() );
    }
    ASSERT_FALSE( addresses.empty());
    ASSERT_TRUE( fd.open(addresses[0]) );
#if CX_PLATFORM == CX_P_WINDOWS
    ASSERT_TRUE( fd.handle()->fd.s != INVALID_SOCKET );
#endif
    ASSERT_TRUE(fd.connect( addresses[0]));

    std::string get("GET / HTTP/1.1\r\n\r\n");
    cx::io::ip::tcp::buffer buf(get.data() , get.length());
    ASSERT_EQ( fd.write( buf ) , buf.length() );
    char read_buf[1024] = {0 , };
    cx::io::ip::tcp::buffer rdbuf(read_buf , 1024);
    ASSERT_TRUE( fd.read( rdbuf ) > 0 );
    gprintf( "%s" , rdbuf.base());
    fd.close();
#if CX_PLATFORM == CX_P_WINDOWS
    ASSERT_TRUE( fd.handle()->fd.s == INVALID_SOCKET );
#endif
    ASSERT_TRUE( fd.handle().get() != nullptr );
}