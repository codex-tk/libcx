/**
 */
#ifndef __cx_io_detail_completion_port_socket_h__
#define __cx_io_detail_completion_port_socket_h__

#include <cx/cxdefine.hpp>
#include <cx/io/ip/basic_address.hpp>
#include <cx/io/basic_buffer.hpp>

#if CX_PLATFORM == CX_P_WINDOWS

namespace cx::io {

namespace detail {
    class completion_port;
}

namespace ip::detail {

    template < int Type , int Proto >
    class completion_port_socket_service ;
        
    template <> class completion_port_socket_service< SOCK_STREAM , IPPROTO_TCP >{
    public:
        using implementation_type = cx::io::detail::completion_port;
        using handle_type = cx::io::detail::completion_port::handle_type;
        using address_type = cx::io::ip::basic_address< struct sockaddr_storage , SOCK_STREAM , IPPROTO_TCP >;
        using buffer_type = cx::io::buffer;

        completion_port_socket_service(implementation_type& impl)
            : _implementation( impl )
        {}
        implementation_type& implementation( void ) {
            return _implementation;
        }

        bool open( handle_type& handle , const address_type& address ){
            close(handle);
            handle->fd.s = ::WSASocketW( address.family() 
                        , address.type() 
                        , address.proto() 
                        , nullptr
                        , 0 
                        , WSA_FLAG_OVERLAPPED );
            if ( handle->fd.s != INVALID_SOCKET )  
                return true;
            return false;
        }

        void close( handle_type& handle ) {
            ::closesocket( handle->fd.s );
            handle->fd.s = INVALID_SOCKET;
        }

        int write( handle_type& handle , const buffer_type& buf ){ 
            return 0;
        }

        int read( handle_type& handle , buffer_type& buf ){ 
           return 0;
        }
    private:
        implementation_type& _implementation;
    };

    template <> class completion_port_socket_service< SOCK_DGRAM , IPPROTO_UDP>{
    public:
        using implementation_type = cx::io::detail::completion_port;
        using handle_type = cx::io::detail::completion_port::handle_type;
        using address_type = cx::io::ip::basic_address< struct sockaddr_storage , SOCK_DGRAM , IPPROTO_UDP >;
        struct _buffer_type {
            address_type address;
            cx::io::buffer buffer;
        };
        using buffer_type = _buffer_type;

        completion_port_socket_service(implementation_type& impl)
            : _implementation( impl )
        {}
        implementation_type& implementation( void ) {
            return _implementation;
        }
        bool open( handle_type& handle , const address_type& address ){
            close(handle);
            handle->fd.s = ::WSASocketW( address.family() 
                        , address.type() 
                        , address.proto() 
                        , nullptr
                        , 0 
                        , WSA_FLAG_OVERLAPPED );
            if ( handle->fd.s != INVALID_SOCKET )  
                return true;
            return false;
        }

        void close( handle_type& handle ) {
            ::closesocket( handle->fd.s );
            handle->fd.s = INVALID_SOCKET;
        }

        int write( handle_type& handle , const buffer_type& buf ){ 
            return 0;
        }

        int read( handle_type& handle , buffer_type& buf ){ 
            return 0;
        }
    private:
        implementation_type& _implementation;
    };

}}

#endif // 

#endif