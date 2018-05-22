/**
 * @brief 
 * 
 * @file basic_address.hpp
 * @author ghtak
 * @date 2018-05-22
 */
#ifndef __cx_io_ip_basic_address_h__
#define __cx_io_ip_basic_address_h__

#if defined( _WIN32 ) || defined( _WIN64 )
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment( lib , "ws2_32")
namespace cx::io::ip::detail{
    namespace {
        struct win32_socket_initializer {
            win32_socket_initializer(void){
                WSADATA    wsaData;
                WSAStartup(MAKEWORD(2,2), &wsaData);
            }
        };
        static win32_socket_initializer initializer;
    }

}

#else

#endif

namespace cx::io::ip {

    template < int Family , int Type , int Proto >
    class basic_address {
    public:
        basic_address( void )
            : _size(0) {
             memset( &_address , 0x00 , sizeof( _address ));
        }

        basic_address( const basic_address& rhs )
            : _size(rhs.size()) {
            memcpy( address()  , rhs.address() , rhs.size());
        }

        ~basic_address( void ) {
        }
        
        struct sockaddr* address( void ) {
            return const_cast< sockaddr* >(
                static_cast< const basic_address *>(this)->address());
        }

        const struct sockaddr* address( void ) const {
            return reinterpret_cast< const struct sockaddr* >( &_address );
        }

        int size( void ) const {
            return _size;
        }

        int* size_ptr( void ) {
            return &_size;
        }
    private:
        struct sockaddr_storage _address;
        int _size;
    };
#if defined( _WIN32 ) || defined( _WIN64 )
#else
    template <int Type , int Proto>
    class basic_address< PF_FILE , Type , Proto >   {
    public:
    
    private:
        struct sockaddr_un _address;
        socklen_t _length;
    };
#endif
}

#endif