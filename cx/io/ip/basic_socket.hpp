/**
 * @brief 
 * 
 * @file basic_socket.hpp
 * @author ghtak
 * @date 2018-05-22
 */
#include <cx/io/ip/basic_address.hpp>

namespace cx::io::ip{

    template < int Family , int Type , int Proto >
    class basic_socket {
    public:
        using address = basic_address< Family , Type , Proto >;
#if defined( _WIN32 ) || defined (_WIN64)
        using descriptor_type = SOCKET;
        static const descriptor_type invalid_descriptor = INVALID_SOCKET;
#else
        using descriptor_type = int;
        static const descriptor_type invalid_descriptor = -1;
#endif   
        basic_socket( void ) : _fd(invalid_descriptor){}

        bool open( void ) {
#if defined( _WIN32 ) || defined (_WIN64)
            _fd = WSASocketW( Family , Type , Proto , nullptr , 0 , WSA_FLAG_OVERLAPPED  );
#else
            
#endif   
            return _fd != invalid_descriptor;
        }

        void close( void ) {
            if ( _fd != invalid_descriptor ) {
#if defined( _WIN32 ) || defined (_WIN64)
                closesocket( _fd );
#else
            
#endif      
            }
            _fd = invalid_descriptor;
        }

        descriptor_type descriptor( void ) {
            return _fd;
        }

    private:
        descriptor_type _fd;
    };

    namespace tcp {
        inline namespace v4 {
            using socket = basic_socket< AF_INET , SOCK_STREAM , IPPROTO_TCP >;
            using address = socket::address;
        }

        namespace v6{
            using socket = basic_socket< AF_INET6, SOCK_STREAM , IPPROTO_TCP >;
            using address = socket::address;
        }
    }
} // cx::io::ip

