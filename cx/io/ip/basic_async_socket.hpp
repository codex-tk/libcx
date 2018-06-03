/**
 * @brief 
 * 
 * @file basic_async_socket.hpp
 * @author ghtak
 * @date 2018-06-03
 */
#include <cx/io/ip/basic_address.hpp>
#include <cx/io/async_layer.hpp>
#include <cx/io/async_layer_impl/basic_connect.hpp>
#include <cx/io/async_layer_impl/basic_write.hpp>
#include <cx/io/async_layer_impl/basic_read.hpp>
namespace cx::io::ip {

    template < int Type , int Proto >
    class basic_async_socket {
    public:
        template < typename HandlerT >
        using connect_op = cx::io::ip::detail::basic_connect< HandlerT , Type , Proto >;
        template < typename HandlerT >
        using write_op = cx::io::ip::detail::basic_write< HandlerT , Type , Proto >;
        template < typename HandlerT >
        using read_op = cx::io::ip::detail::basic_read< HandlerT , Type , Proto >;

        using buffer_type = typename cx::io::ip::socket_layer<Type,Proto>::buffer_type;

        basic_async_socket( cx::io::engine& engine  )
            : _handle( engine.async_layer().make_shared_handle() )
        {}

        ~basic_async_socket( void ){}

        template < typename HandlerT >
        void connect( const cx::io::ip::address& addr , HandlerT&& handler ){
            connect_op<HandlerT>* op = new connect_op<HandlerT>( addr , std::forward<HandlerT>(handler));
            _handle->layer.connect( _handle , op );
        }

        void close( void ) {
            _handle->layer.closesocket( _handle );
            _handle = nullptr;
        }

        template < typename HandlerT >
        void write( const buffer_type& buffer ,  HandlerT&& handler ){
            write_op< HandlerT >* op = new write_op< HandlerT >( buffer ,  std::forward<HandlerT>(handler));
            _handle->layer.write( _handle , op );
        }

        template < typename HandlerT >
        void read( const buffer_type& buffer ,  HandlerT&& handler ){
            read_op< HandlerT >* op = new read_op< HandlerT >( buffer ,  std::forward<HandlerT>(handler));
            _handle->layer.read( _handle , op );
        }
    private:
        cx::io::async_layer::handle_ptr _handle;
    };

}