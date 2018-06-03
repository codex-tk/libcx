/**
 * @brief 
 * 
 * @file basic_async_connector.hpp
 * @author ghtak
 * @date 2018-06-03
 */
#include <cx/io/ip/basic_async_socket.hpp>

namespace cx::io::ip {
/*
    template < int Type, int Proto >
    class basic_async_connector {
    public:
        basic_async_connector( async_layer& l )
            : _layer(l)
        {}

        template < typename HandlerT >
        void connect( const cx::io::ip::address& addr , HandlerT&& handler ) {
            basic_async_socket< Type , Proto > fd(_layer);
            if ( fd.open( addr.family() ) ) {
                fd.set_option( cx::io::ip::option::non_block());
                fd.connect( addr , std::forward<HandlerT>(handler) );
            }
        }
    private:
        async_layer& _layer;
    };*/
}