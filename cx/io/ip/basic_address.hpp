/**
 * @brief 
 * 
 * @file address.hpp
 * @author ghtak
 * @date 2018-05-26
 */
#ifndef __cx_io_ip_address_h__
#define __cx_io_ip_address_h__

#include <cx/cxdefine.hpp>

#if CX_PLATFORM == CX_P_WINDOWS

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

#endif

namespace cx::io::ip{

    template < typename SockAddrT >
    class basic_address {
    public:
        basic_address( void ) : _length(0) {
            memset( &_address , 0x00 , sizeof( _address ));
        }

        basic_address( struct sockaddr* addr_ptr , const int length )
            : _length(length) {
            memcpy( sockaddr()  , addr_ptr , _length );
        }

        basic_address( const basic_address& rhs ) : _size(rhs.length()) {
            memcpy( sockaddr()  , rhs.sockaddr() , _length );
        }

        ~basic_address( void ) {
        }
        
        struct sockaddr* sockaddr( void ) {
            return const_cast< struct sockaddr* >(
                static_cast< const basic_address *>(this)->sockaddr());
        }

        const struct sockaddr* sockaddr( void ) const {
            return reinterpret_cast< const struct sockaddr* >( &_address );
        }

        int length( void ) const {
            return _length;
        }

        int* length_ptr( void ) {
            return &_length;
        }
    public:
        static basic_address any( const uint16_t port , const short family = AF_INET ) {
            sockaddr_in addr;
            memset( &addr , 0x00 , sizeof(addr));
            addr.sin_family = family; 
            addr.sin_port = htons( port ); 
            addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
            return address( reinterpret_cast< struct sockaddr*>(&addr)
                , sizeof(addr));
        }

        static std::vector< basic_address > resolve( const std::string& u8addr ){

            return std::vector< basic_address >();
        }

    private:
        SockAddrT _address;
        int _length;
    };

    using address = basic_address< struct sockaddr_storage >;
}

#endif