#include <cx/io/basic_buffer.hpp>
namespace cx::io {

    namespace detail{
#if CX_PLATFORM == CX_P_WINDOWS
        void* buffer_base( const WSABUF* buf ) {
            return buf->buf; 
        }
        void* buffer_base( WSABUF* buf , void* ptr ) { 
            void* old = buf->buf;
            buf->buf = static_cast< decltype(buf->buf) >(ptr);
            return old; 
        }
        std::size_t buffer_length( const WSABUF* buf ) {
            return buf->len;
        }
        std::size_t buffer_length( WSABUF* buf , std::size_t len ) { 
            std::size_t old = buf->len;
            buf->len = static_cast< decltype(buf->len) >(len);
            return old; 
        }
#else
        void* buffer_base( const iovec* buf ) {
            return buf->iov_base; 
        }
        void* buffer_base( iovec* buf , void* ptr ) { 
            void* old = buf->iov_base;
            buf->iov_base = static_cast< decltype(buf->iov_base) >(ptr);
            return old; 
        }
        std::size_t buffer_length( const iovec* buf ) {
            return buf->iov_len;
        }
        std::size_t buffer_length( iovec* buf , std::size_t len ) { 
            std::size_t old = buf->iov_len;
            buf->iov_len = static_cast< decltype(buf->iov_len) >(len);
            return old; 
        }
#endif
    }
}