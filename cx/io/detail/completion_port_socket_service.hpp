/**
 */
#ifndef __cx_io_detail_completion_port_socket_h__
#define __cx_io_detail_completion_port_socket_h__

#include <cx/cxdefine.hpp>
#include <cx/io/ip/basic_address.hpp>

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
        using address_type = cx::io::ip::v2::basic_address< SOCK_STREAM , IPPROTO_TCP >;

        completion_port_socket_service(implementation_type& impl)
            : _implementation( impl )
        {}
        implementation_type& implementation( void ) {
            return _implementation;
        }
    private:
        implementation_type& _implementation;
    };

    template <> class completion_port_socket_service< SOCK_DGRAM , IPPROTO_UDP>{
    public:
        using implementation_type = cx::io::detail::completion_port;
        using handle_type = cx::io::detail::completion_port::handle_type;
        using address_type = cx::io::ip::v2::basic_address< SOCK_DGRAM , IPPROTO_UDP >;

        completion_port_socket_service(implementation_type& impl)
            : _implementation( impl )
        {}
        implementation_type& implementation( void ) {
            return _implementation;
        }
    private:
        implementation_type& _implementation;
    };

}}

#endif // 

#endif