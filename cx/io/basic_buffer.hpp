#ifndef __cx_io_basic_buffer_h__
#define __cx_io_basic_buffer_h__

#include <cx/cxdefine.hpp>

namespace cx::io {

    namespace detail{
#if CX_PLATFORM == CX_P_WINDOWS
        void* buffer_ptr( const WSABUF* buf );
        void* buffer_ptr( WSABUF* buf , void* ptr );
        std::size_t buffer_len( const WSABUF* buf );
        std::size_t buffer_len( WSABUF* buf , std::size_t len );
#else
        void* buffer_ptr( const iovec* buf );
        void* buffer_ptr( iovec* buf , void* ptr );
        std::size_t buffer_len( const iovec* buf );
        std::size_t buffer_len( iovec* buf , std::size_t len );
#endif
    }

    template < typename T > class basic_buffer : public T {
    public:
        basic_buffer( void* ptr , std::size_t len ) {
            detail::buffer_ptr( raw_buffer() ,  ptr );
            detail::buffer_len( raw_buffer() ,  len );
        }

        basic_buffer( const std::string_view& msg ) {   
            detail::buffer_ptr( raw_buffer() ,  const_cast<char*>(msg.data()) );
            detail::buffer_len( raw_buffer() ,  msg.size() );
        }

        void* ptr( void ) const { return detail::buffer_ptr( raw_buffer()) }
        std::size_t len( void ) const { return detail::buffer_len( raw_buffer()); }
        void* ptr( void* new_ptr ) { 
            return detail::buffer_ptr( raw_buffer() , new_ptr );
        }
        std::size_t len( const std::size_t new_size ) {
            return detail::buffer_len( raw_buffer() , new_size );
        }

        T* raw_buffer( void ) {
            return static_cast< T* >(this);
        }
    };

#if CX_PLATFORM == CX_P_WINDOWS
    using buffer = basic_buffer< WSABUF >;
#else

#endif
}

#endif
