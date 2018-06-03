/**
 * @brief 
 * 
 * @file iocp_async_layer.hpp
 * @author ghtak
 * @date 2018-06-03
 */
#ifndef __cx_io_detail_iocp_async_layer_h__
#define __cx_io_detail_iocp_async_layer_h__

#include <memory>
#include <cx/cxdefine.hpp>
#include <cx/core/function.hpp>
#include <cx/core/allocators.hpp>

#if CX_PLATFORM == CX_P_WINDOWS

namespace cx::io {

    class engine;

namespace detail {
    
    class iocp_async_layer {
    public:
        iocp_async_layer( cx::io::engine& e ) 
            : _engine( e ){
        }

        struct handle;
        using handle_ptr = std::shared_ptr< iocp_async_layer::handle >;

        handle_ptr make_shared_handle( void ) {
            return std::make_shared< iocp_async_layer::handle >( *this );
        }

        struct handle{
            handle( iocp_async_layer& l ) : layer(l){}
            iocp_async_layer& layer;
        };

        class connect_op {
        public:
            template < typename T >
            connect_op( T&& h ) 
                : _function(std::forward<T>(h)){}
            ~connect_op( void ){}
            void operator()(const std::error_code& ec){
                _function(ec);
            }
        private:
            cx::core::function< void ( const std::error_code& ec ) > _function;
        };

        void connect( handle_ptr ptr , connect_op * op ) {
            (*op)( std::error_code());
            cx::core::async_op_allocator<connect_op> _a1;
            _a1.destroy(op);
            _a1.deallocate( op , 1 );
        }

         cx::io::engine& engine( void ) {
             return _engine;
         }
    private:
        cx::io::engine& _engine;
    };
}
}
#endif

#endif