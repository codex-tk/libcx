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
#include <cx/io/async_layer_impl/basic_connect.hpp>

#if CX_PLATFORM == CX_P_WINDOWS

namespace cx::io { class engine; }
namespace cx::io::detail {
    
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

        void connect( handle_ptr ptr , cx::io::ip::detail::base_connect * op ) {
            (*op)( std::error_code());
            delete op;
        }

         cx::io::engine& engine( void ) {
             return _engine;
         }
    private:
        cx::io::engine& _engine;
    };
}
#endif

#endif