/**
 * @brief 
 * 
 * @file basic_async_socket.hpp
 * @author ghtak
 * @date 2018-06-03
 */
#include <cx/io/ip/basic_address.hpp>
#include <cx/io/async_layer.hpp>
#include <cx/core/allocators.hpp>

namespace cx::io::ip {

    template < int Type , int Proto >
    class basic_async_socket {
    public:
        basic_async_socket( cx::io::engine& engine  )
            : _handle( engine.async_layer().make_shared_handle() )
        {}

        ~basic_async_socket( void ){}

        template < typename HandlerT >
        void connect( const cx::io::ip::address& addr , HandlerT&& handler ){
            cx::core::async_op_allocator<async_layer::connect_op> _a1;
            async_layer::connect_op* op = _a1.allocate( 1 );
            _a1.construct( op , std::forward<HandlerT>(handler) );
            _handle->layer.connect( _handle , op );
        }
    private:
        cx::io::async_layer::handle_ptr _handle;
    };

}