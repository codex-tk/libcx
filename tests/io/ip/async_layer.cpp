#include "tests/gprintf.hpp"
#include <cx/io/engine.hpp>
#include <cx/io/ip/basic_async_socket.hpp>
#include <cx/io/ip/basic_async_acceptor.hpp>

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
            if ( ec ) 
                testValue = -1;
            else
                testValue = 1;
        });
    ASSERT_EQ( engine.async_layer().run( std::chrono::milliseconds(1000)) , 1);
    ASSERT_EQ( testValue , 1 );

    auto get = cx::io::ip::basic_async_socket< SOCK_STREAM , IPPROTO_TCP >::buffer_type( "GET / HTTP/1.1\r\n\r\n" );
    fd.write( get , [&] ( const std::error_code& ec , int io_size ){
        if ( ec ) 
            testValue = -1;
        else
            testValue = io_size;
    });
    ASSERT_EQ( engine.async_layer().run( std::chrono::milliseconds(1000)) , 1);
    ASSERT_EQ( testValue , get.len() );

    char read_buffer[1024] = { 0 , };
    cx::io::ip::basic_async_socket< SOCK_STREAM , IPPROTO_TCP >::buffer_type rdbuf( read_buffer , 1023 );
    fd.read( rdbuf , [&] ( const std::error_code& ec , int io_size ){
        if ( ec ) 
            testValue = -1;
        else
            testValue = io_size;
    });
    ASSERT_EQ( engine.async_layer().run( std::chrono::milliseconds(1000)) , 1);
    ASSERT_TRUE( testValue > 0 );

    gprintf( "Recv %s" , read_buffer );
    fd.close();
}

TEST( async_layer , accept ){
    cx::io::engine e;
    cx::io::ip::basic_async_acceptor< SOCK_STREAM , IPPROTO_TCP > acceptor(e);
}