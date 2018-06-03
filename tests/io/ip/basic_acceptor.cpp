/**
 */

#include <cx/io/ip/basic_acceptor.hpp>
#include <cx/io/ip/basic_connector.hpp>
#include <thread>

#include "tests/gprintf.hpp"


TEST( cx_io_ip_basic_acceptor , open ) {
    cx::io::ip::tcp::acceptor acceptor;
    ASSERT_TRUE(acceptor.open( cx::io::ip::address::any( 7543 , AF_INET ) ));
    std::thread th([&]() {
        cx::io::ip::address addr;
        acceptor.accept( addr , std::chrono::milliseconds(4000)).close();;
    });
    
    cx::io::ip::tcp::connector conn;
    cx::io::ip::tcp::socket fd(conn.connect(cx::io::ip::address( AF_INET , "127.0.0.1" , 7543), std::chrono::milliseconds(500)));
    ASSERT_TRUE( fd.handle() != cx::io::ip::invalid_socket );
    fd.close();
    th.join();
    acceptor.close();
}

TEST( cx_io_ip_basic_acceptor , echo ) {
    cx::io::ip::tcp::acceptor acceptor;
    ASSERT_TRUE(acceptor.open(  cx::io::ip::address::any( 7543 , AF_INET )   ));
    std::thread th([&]() {
        while( true ){
            cx::io::ip::address addr;   
            auto ss = acceptor.accept( addr , std::chrono::milliseconds(100));
            if ( ss.handle() != cx::io::ip::invalid_socket ) {
                auto fd = ss.handle( cx::io::ip::invalid_socket );
                std::thread thr([fd] {
                    cx::io::ip::tcp::socket s(fd);
                    cx::io::selector<cx::io::ip::socket_type, 1> selector;
                    selector.bind(s.handle(), cx::io::ops::read, 0);
                    while (selector.select(1000) > 0) {
                        char ch;
                        int len = s.read(cx::io::basic_buffer(&ch, 1));
                        if ( len <= 0 ) 
                            break;
                        s.write(cx::io::basic_buffer(&ch, 1));
                        if (ch == 'f')
                            break;
                    }
                    s.close();
                });
                thr.detach();
            } else {
                break;
            }
        }
    });

    std::array< cx::io::ip::tcp::socket , 5 > fds;
    cx::io::ip::tcp::connector conn;
    for ( auto& fd : fds ) {
        fd = conn.connect(  cx::io::ip::address( AF_INET , "127.0.0.1" , 7543) , std::chrono::milliseconds(1000));
        ASSERT_TRUE( fd.handle() != cx::io::ip::invalid_socket );
        fd.set_option( cx::io::ip::option::blocking());
    }
    std::string send_data("abcdef");
    for ( std::size_t i = 0 ; i < send_data.length() ; ++i ){
        for ( auto& fd : fds ) {
            ASSERT_TRUE( fd.handle() != cx::io::ip::invalid_socket );
            ASSERT_EQ( fd.write( cx::io::basic_buffer( &(send_data[i]) , 1 )) , 1 );
            char ch;
            ASSERT_EQ( fd.read( cx::io::basic_buffer(&ch ,1)) , 1 );
            ASSERT_EQ( ch , send_data[i]);
        }
    }
    for ( auto& fd : fds ) {
        fd.close();
    }
    th.join();
    acceptor.close();
}