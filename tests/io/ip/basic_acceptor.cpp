/**
 */

#include <cx/io/ip/basic_acceptor.hpp>
#include <thread>

#include "tests/gprintf.hpp"

class acceptor_handler {
public:    
    bool select_address( const cx::io::ip::address& addr ) {
        return true;
    }
    void handle_accept( cx::io::ip::socket_type fd ,  const cx::io::ip::address& addr  ) {
        cx::io::ip::tcp::socket s(fd);
        s.close();
    }
};


TEST( cx_io_ip_basic_acceptor , open ) {
    acceptor_handler handler;
    cx::io::ip::tcp::acceptor<acceptor_handler> acceptor(handler);
    ASSERT_TRUE(acceptor.open( 7543 , AF_UNSPEC ));
    std::thread th([&]() {
        acceptor.accept( std::chrono::milliseconds(4000));
    });
    
    cx::io::ip::tcp::socket fd;
    fd.open( AF_INET );
    cx::io::ip::tcp::socket::implementation::connect( fd.handle() , cx::io::ip::address( AF_INET , "127.0.0.1" , 7543));
    fd.close();
    th.join();
    acceptor.close();
}