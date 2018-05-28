#ifndef __cx_io_h__
#define __cx_io_h__

#include <cx/cxdefine.hpp>

#if CX_PLATFORM == CX_P_WINDOWS

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <windows.h>
#include <stdint.h>

#pragma comment( lib , "ws2_32.lib")
#pragma comment( lib , "Mswsock.lib") 
#pragma comment( lib , "IPHLPAPI.lib")

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

#include <sys/uio.h>

#endif
namespace cx::io {

#if CX_PLATFORM == CX_P_WINDOWS
    using descriptor_type = SOCKET;
    static const descriptor_type invalid_descriptor = INVALID_SOCKET;
#else
    using descriptor_type = int;
    static const descriptor_type invalid_descriptor = -1;
#endif   

    namespace ops {
        enum {
            read = 0x01 ,
            write = 0x02 ,
        };
    }

     
#if CX_PLATFORM == CX_P_WINDOWS
    class buffer : public WSABUF {
    public:
        void* ptr( void ) const { return this->buf; }
        std::size_t len( void ) const { return static_cast< const WSABUF*>(this)->len; }
        void* ptr( const void* new_ptr ) { 
            void* old = this->buf;
            this->buf = static_cast< char* >( 
                const_cast< void*>(new_ptr));
            return old;
        }
        std::size_t len( const std::size_t new_size ) {
            std::size_t old = this->len();
            static_cast<WSABUF*>(this)->len = new_size;
            return old;
        }
#else
    class buffer : public struct iovec {
    public:
        void* ptr( void ) const { return this->iov_base; }
        std::size_t len( void ) const { return this->iov_len; }
        void* ptr( void* new_ptr ) { 
            void* old = this->iov_base;
            this->iov_base = static_cast< char* >(new_ptr);
            return old;
        }
        std::size_t len( std::size_t new_size ) {
            std::size_t old = this->len();
            this->iov_len = new_size;
            return old;
        }
#endif
        buffer( void* ptr , std::size_t len ) {
            this->ptr(ptr);
            this->len(len);
        }
        buffer( const std::string_view& msg ) {
            this->ptr( msg.data());
            this->len( msg.size());
        }
    };
}

#endif