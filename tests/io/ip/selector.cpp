/**
 * @brief 
 * 
 * @file selector.cpp
 * @author ghtak
 * @date 2018-05-28
 */
#include "gtest/gtest.h"
#include "tests/gprintf.hpp"
#include <cx/io/ip/basic_socket.hpp>
#include <cx/io/ip/option.hpp>
#include <cx/io/selector.hpp>

using namespace cx;
using namespace io;

TEST( cx_io_selector , to  ) {
    ip::tcp::socket server;
    ASSERT_TRUE( server.open() );
    ASSERT_TRUE( server.set_option( ip::option::reuse_address(ip::option::enable)));
    ASSERT_TRUE( server.set_option( ip::option::non_blocking()));
    ASSERT_TRUE( server.bind(ip::address::any(7543 , AF_INET )[0]));
    ASSERT_TRUE( ip::tcp::socket::implementation::listen( server.handle()) );

    ip::tcp::socket client;
    ip::address clientaddr( AF_INET , "127.0.0.1" , 7543 );
    ASSERT_TRUE( client.open() );
    ASSERT_TRUE( client.set_option( ip::option::non_blocking()));
    ASSERT_TRUE( ip::tcp::socket::implementation::connect( client.handle() , clientaddr ));

    cx::io::selector< cx::io::ip::socket_type , 32 > selector;
    selector.bind( server.handle() , cx::io::ops::read , nullptr );
    selector.bind( client.handle() , cx::io::ops::read | cx::io::ops::write , nullptr );
    ASSERT_EQ( selector.select(4000) , 2);

    cx::io::selector< cx::io::ip::socket_type , 32 >::iterator it(selector);
    while ( it ) {
        cx::io::ip::socket_type fd = it.handle();
        int ops = it.ops();
        void* ctx = it.udata();
        if ( fd == server.handle() ) {
            ASSERT_EQ( ops , cx::io::ops::read );
            ASSERT_EQ( ctx , nullptr );
        }
        if ( fd == client.handle() ) {
            ASSERT_EQ( ops , cx::io::ops::write );
            ASSERT_EQ( ctx , nullptr );
        }
        ++it;
    }

    client.close();
    server.close();
}
