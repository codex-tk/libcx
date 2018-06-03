/**
 */
#ifndef __cx_io_ip_basic_read_h__
#define __cx_io_ip_basic_read_h__

#include <cx/io/ip/socket_layer.hpp>
#include <cx/io/async_layer_impl/base_op.hpp>

namespace cx::io::ip::detail {

    template < int Type , int Proto >
    class base_read : public base_op {
    public:
        using buffer_type = typename cx::io::ip::socket_layer< Type , Proto >::buffer_type;
        base_read( const buffer_type& buf )
            : _buffer(buf)
        {

        }
        buffer_type& buffer( void ) {
            return _buffer;
        }
    private:
        buffer_type _buffer;
    };

    template < typename HandlerT , int Type , int Proto >
    class basic_read : public base_read<Type,Proto> {
    public:
        basic_read( const buffer_type& buf , HandlerT&& h ) 
            : base_read(buf) 
            , _handler( std::forward<HandlerT>(h)) 
        {
        }
        virtual ~basic_read( void ) {
        }
        virtual void operator()(void) override {
            _handler(error() , io_size());
        }
    private:
        HandlerT _handler;
    };

}

#endif