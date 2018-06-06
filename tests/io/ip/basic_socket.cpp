
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
    ASSERT_TRUE( fd.open(addr) );
#if CX_PLATFORM == CX_P_WINDOWS
    ASSERT_TRUE( fd.handle()->fd.s != INVALID_SOCKET );
#endif
    fd.close();
#if CX_PLATFORM == CX_P_WINDOWS
    ASSERT_TRUE( fd.handle()->fd.s == INVALID_SOCKET );
#endif
    ASSERT_TRUE( fd.handle().get() != nullptr );
}