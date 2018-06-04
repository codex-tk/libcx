/**
 * @brief 
 * 
 * @file basic_async_acceptor.hpp
 * @author ghtak
 * @date 2018-06-04
 */
#include <cx/io/ip/basic_address.hpp>
#include <cx/io/async_layer.hpp>

#ifndef __cx_io_ip_basic_async_acceptor_h__
#define __cx_io_ip_basic_async_acceptor_h__

namespace cx::io::ip {

    template < int Type , int Proto >
    class basic_async_acceptor {
    public:
        basic_async_acceptor( cx::io::engine& e ) 
            : _handle(e.async_layer().make_shared_handle())
        {   
        }
        
        bool open( const cx::io::ip::address& address ){
            return _handle->layer.open( _handle , , Type , Proto , address );
        }

        void close( void ){

        }

    private:
        cx::io::async_layer::handle_ptr _handle;
    };

}

#endif