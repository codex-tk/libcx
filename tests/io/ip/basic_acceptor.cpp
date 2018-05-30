/**
 */

#include <cx/io/ip/basic_acceptor.hpp>

#include "tests/gprintf.hpp"

TEST( cx_io_ip_basic_acceptor , open ) {
    cx::io::ip::tcp::acceptor acceptor;
    ASSERT_TRUE(acceptor.open( 7543 , AF_UNSPEC ));
    acceptor.handle_accept( [] {} ,std::chrono::milliseconds(4000));
    acceptor.close();
}