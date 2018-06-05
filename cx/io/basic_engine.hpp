/**
 */
#ifndef __cx_io_basic_engine_h__
#define __cx_io_basic_engine_h__

#include <cx/service_repository.hpp>

#include <cx/io/basic_object.hpp>

#include <cx/io/detail/completion_port.hpp>
#include <cx/io/detail/completion_port_socket_service.hpp>
namespace cx::io {

   
#if CX_PLATFORM == CX_P_WINDOWS
    using basic_implementation = cx::io::detail::completion_port;

    namespace ip::tcp {
        using service = cx::io::ip::detail::completion_port_socket_service<SOCK_STREAM,IPPROTO_TCP>;
        using socket = cx::io::basic_object<service>;
    }
    namespace ip::udp { 
        using service = cx::io::ip::detail::completion_port_socket_service<SOCK_DGRAM,IPPROTO_UDP>; 
        using socket = cx::io::basic_object<service>;
    }
#elif CX_PLATFORM == CX_P_LINUX
    using implementation_type = cx::io::detail::epoll;
#elif CX_PLATFORM == CX_P_MACOSX
    using implementation_type = cx::io::detail::kqueue;
#else
    using implementation_type = cx::io::detail::poll;
#endif

    template < typename ImplementationType , typename ... Services >
    class basic_engine {
    public:
        using implementation_type = ImplementationType;
        basic_engine( void )
            : _services(_implementation)
        {}
        
        ~basic_engine( void ){
        }

        template < typename Service >
        Service& service( void ) {
            return _services.service< Service >();
        }

        implementation_type& implementation( void ) {
            return _implementation;
        }
    private:
        implementation_type _implementation;
        cx::service_repository< Services ... > _services;
    };

}

#endif