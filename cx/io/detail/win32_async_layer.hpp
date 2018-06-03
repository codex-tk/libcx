/**
 * @brief 
 * 
 * @file win32_async_layer.hpp
 * @author ghtak
 * @date 2018-06-03
 */
#ifndef __cx_io_detail_win32_async_layer_h__
#define __cx_io_detail_win32_async_layer_h__

#include <memory>
#include <cx/cxdefine.hpp>

#if CX_PLATFORM == CX_P_WINDOWS

namespace cx::io::detail {
    
    class win32_async_layer {
    public:
        struct handle;
        using handle_ptr = std::shared_ptr< win32_async_layer::handle >;

        handle_ptr make_shared_handle( void ) {
            return std::make_shared< win32_async_layer::handle >( *this );
        }

        struct handle{
            handle( win32_async_layer& l ) : layer(l){}

            win32_async_layer& layer;
        };
    private:
    };
}
#endif

#endif