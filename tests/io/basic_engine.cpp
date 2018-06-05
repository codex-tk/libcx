#include "tests/gprintf.hpp"
#include <cx/io/basic_engine.hpp>
#include <cx/cxdefine.hpp>

class impl {
public:
    impl( void ){ handle = 32;}
    int handle;
};

class foo_service {
public:
    foo_service( cx::io::basic_implementation& impl ) 
        : _impl(impl)
    {

    }
   cx::io::basic_implementation& _impl;
};


TEST( basic_engine , t0 ) {
    cx::io::basic_engine< cx::io::basic_implementation , foo_service > engine;
    ASSERT_EQ(engine.service<foo_service>()._impl.run( std::chrono::milliseconds(1)) , 0 );
    int value = 0;
    engine.implementation().post_handler([&] {
        ++value;
    });
    engine.implementation().post_handler([&] {
        ++value;
    });
    ASSERT_EQ(engine.service<foo_service>()._impl.run( std::chrono::milliseconds(1)) , 2 );
    ASSERT_EQ( value , 2 );
}

TEST( basic_engine , socket_service ) {
    cx::io::basic_engine< 
        cx::io::basic_implementation 
        , cx::io::ip::tcp::service
        , cx::io::ip::udp::service > engine;
    ASSERT_EQ(engine.implementation().run( std::chrono::milliseconds(1)) , 0 );
    int value = 0;
    engine.service<cx::io::ip::tcp::service>().implementation().post_handler([&] {
        ++value;
    });
    engine.service<cx::io::ip::udp::service>().implementation().post_handler([&] {
        ++value;
    });
    ASSERT_EQ(engine.service<cx::io::ip::tcp::service>().implementation().run( std::chrono::milliseconds(1)) , 2 );
    ASSERT_EQ( value , 2 );
}


TEST( union_test , to ) {
    struct handle {
        union {
            SOCKET s;
            HANDLE h;
        } fd;
    };

    handle h;
    h.fd.s = INVALID_SOCKET;
}

TEST( basic_engine , tcp ) {
    cx::io::basic_engine< 
        cx::io::basic_implementation
        , cx::io::ip::tcp::service  
        , cx::io::ip::udp::service > engine;
    cx::io::ip::tcp::socket fd(engine);
    ASSERT_TRUE( fd.handle().get() != nullptr );
}