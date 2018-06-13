#include "gtest/gtest.h"
#include "tests/gprintf.hpp"
#include <cx/io/basic_engine.hpp>

TEST( cx_io_ip_udp , ehco ) {
    cx::io::engine< cx::io::ip::udp::service > engine;
    cx::io::ip::udp::socket svr_fd( engine );
    cx::io::ip::udp::socket cli_fd( engine );

    cx::io::ip::udp::address addr = cx::io::ip::udp::address::any( 7543 , AF_INET  );

    ASSERT_TRUE( svr_fd.open( addr ));
    ASSERT_TRUE( svr_fd.bind( addr ));

    std::string hello("Hello");

    cx::io::ip::udp::buffer buf;
    buf.address = cx::io::ip::udp::address( "127.0.0.1" , 7543 , AF_INET );
    buf.buffer = cx::io::buffer( hello.data() , hello.length() );
    ASSERT_TRUE( cli_fd.open( buf.address ));
    ASSERT_EQ( static_cast<std::size_t>(cli_fd.write( buf )) , buf.buffer.length() );

    char read_buf[1024] = {0 , };
    cx::io::ip::udp::buffer rdbuf;
    rdbuf.buffer = cx::io::buffer( read_buf , 1024);
    ASSERT_TRUE( svr_fd.read( rdbuf ) > 0 );
    gprintf( "%s" , rdbuf.buffer.base() );
    svr_fd.close();
    cli_fd.close();
}


TEST( cx_io_ip_udp , async ) {
    cx::io::engine< cx::io::ip::udp::service > engine;
    cx::io::ip::udp::socket svr_fd( engine );
    cx::io::ip::udp::socket cli_fd( engine );

    cx::io::ip::udp::address addr = cx::io::ip::udp::address::any( 7543 , AF_INET  );

    ASSERT_TRUE( svr_fd.open( addr ));
    ASSERT_TRUE( svr_fd.bind( addr ));

    std::string hello("Hello");

    cx::io::ip::udp::buffer buf;
    buf.address = cx::io::ip::udp::address( "127.0.0.1" , 7543 , AF_INET );
    buf.buffer = cx::io::buffer( hello.data() , hello.length() );
    ASSERT_TRUE( cli_fd.open( buf.address ));
    cli_fd.async_write( buf , [&] ( const std::error_code& ec , int size ) {
        cli_fd.close();
    });

    char read_buf[1024] = {0 , };
    cx::io::ip::udp::buffer rdbuf;
    rdbuf.buffer = cx::io::buffer( read_buf , 1024);
    svr_fd.async_read( rdbuf , [&] ( const std::error_code& ec , int size ) {
        svr_fd.close();
    });
    engine.run();
    gprintf( "%s" , rdbuf.buffer.base() );
}

