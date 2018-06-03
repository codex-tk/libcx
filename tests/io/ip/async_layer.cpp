#include "tests/gprintf.hpp"
#include <cx/io/engine.hpp>
#include <cx/io/ip/basic_async_socket.hpp>

TEST( async_layer , bs ) {
    cx::io::engine engine;
    cx::io::async_layer::handle_ptr hptr = engine.async_layer().make_shared_handle();
}

TEST( async_layer , connect ) {
    cx::io::engine engine;
    cx::io::ip::basic_async_socket< SOCK_STREAM , IPPROTO_TCP > fd(engine);
    int testValue = 0;
    fd.connect(
        cx::io::ip::address( AF_INET , "127.0.0.1" , 7543) 
        , [&] ( const std::error_code& ec ) {
            testValue = 1;
        });
    ASSERT_EQ( testValue , 1 );
}