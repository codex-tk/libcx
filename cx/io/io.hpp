#ifndef __cx_io_h__
#define __cx_io_h__

#include <cx/cxdefine.hpp>

namespace cx::io {

    template < typename T >
    struct handle_trait;
#if CX_PLATFORM == CX_P_WINDOWS
    template <>
    struct handle_trait<HANDLE> {
        using handle_type = HANDLE;
        static constexpr handle_type invalid( void ) { return INVALID_HANDLE_VALUE; }
    };

    template <>
    struct handle_trait<SOCKET> {
        using handle_type = SOCKET;
        static constexpr handle_type invalid( void ) { return INVALID_SOCKET; }
    };
    
#else
    template <>
    struct handle_trait<int> {
        using handle_type = int;
        static constexpr handle_type invalid( void ) { return -1; }
    };
#endif   

    namespace ops {
        enum {
            read = 0x01 ,
            write = 0x02 ,
        };
    }

     
#if CX_PLATFORM == CX_P_WINDOWS
    class basic_buffer : public WSABUF {
#else
    class basic_buffer : public iovec {
#endif
    public:
        basic_buffer( void* ptr , std::size_t len ) {
            this->ptr(ptr);
            this->len(len);
        }
        basic_buffer( const std::string_view& msg ) {            
            this->ptr( const_cast<char*>(msg.data()));
            this->len( msg.size());
        }
#if CX_PLATFORM == CX_P_WINDOWS
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
    };
}

#endif
