#include "tests/gprintf.hpp"
#include <cx/io/engine.hpp>
#include <cx/io/ip/basic_async_socket.hpp>

TEST( async_layer , bs ) {
    cx::io::engine engine;
    cx::io::async_layer::handle_ptr hptr = engine.async_layer().make_shared_handle();
}

TEST( async_layer , connect ) {
    auto addresses = cx::io::ip::address::resolve( "naver.com" , 80 , AF_INET );
   
    ASSERT_FALSE( addresses.empty());
    cx::io::engine engine;
    cx::io::ip::basic_async_socket< SOCK_STREAM , IPPROTO_TCP > fd(engine);
    int testValue = 0;
    fd.connect( addresses[0]
        , [&] ( const std::error_code& ec ) {
            testValue = 1;
        });
    engine.async_layer().run( std::chrono::milliseconds(1000));
    fd.close();
    ASSERT_EQ( testValue , 1 );
}