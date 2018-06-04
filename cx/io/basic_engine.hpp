/**
 */
#ifndef __cx_io_basic_engine_h__
#define __cx_io_basic_engine_h__

#include <cx/service_repository.hpp>

#include <cx/io/detail/completion_port.hpp>
#include <cx/io/detail/completion_port_socket_service.hpp>
namespace cx::io {

    using basic_implementation = 
#if CX_PLATFORM == CX_P_WINDOWS
        cx::io::detail::completion_port;
#elif CX_PLATFORM == CX_P_LINUX
        cx::io::detail::epoll;
#elif CX_PLATFORM == CX_P_MACOSX
        cx::io::detail::kqueue;
#else
        cx::io::detail::poll;
#endif
    namespace ip {
        namespace tcp {
            using service = cx::io::ip::completion_port_socket_service<basic_implementation , SOCK_STREAM,IPPROTO_TCP>;
        }
        namespace udp {
            using service = cx::io::ip::completion_port_socket_service<basic_implementation , SOCK_DGRAM,IPPROTO_UDP>;
        }
    }

    template < typename Implementation , typename ... Services >
    class basic_engine {
    public:
        basic_engine( void )
            : _services(_implementation)
        {}
        
        ~basic_engine( void ){
        }

        template < typename Service >
        Service& service( void ) {
            return _services.service< Service >();
        }

        Implementation& implementation( void ) {
            return _implementation;
        }
    private:
        Implementation _implementation;
        cx::service_repository< Services ... > _services;
    };

}

#endif