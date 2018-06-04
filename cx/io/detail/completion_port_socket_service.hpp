/**
 */
#ifndef __cx_io_detail_completion_port_socket_h__
#define __cx_io_detail_completion_port_socket_h__

#include <cx/cxdefine.hpp>

#if CX_PLATFORM == CX_P_WINDOWS

namespace cx::io {

namespace detail {
    class completion_port;
}

namespace ip {

    template < typename Implementation , int Type , int Proto >
    class completion_port_socket_service ;
        
    template <typename Implementation> class completion_port_socket_service< Implementation , SOCK_STREAM , IPPROTO_TCP >{
    public:
        completion_port_socket_service(Implementation& impl)
            : _implementation( impl )
        {}
        Implementation& implementation( void ) {
            return _implementation;
        }
    private:
        Implementation& _implementation;
    };

    template <typename Implementation> class completion_port_socket_service< Implementation , SOCK_DGRAM , IPPROTO_UDP>{
    public:
        completion_port_socket_service(Implementation& impl)
            : _implementation( impl )
        {}
        Implementation& implementation( void ) {
            return _implementation;
        }
    private:
        Implementation& _implementation;
    };

}}

#endif // 

#endif