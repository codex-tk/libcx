#ifndef __cx_io_basic_buffer_h__
#define __cx_io_basic_buffer_h__

#include <cx/cxdefine.hpp>

namespace cx::io {

    namespace detail{
#if CX_PLATFORM == CX_P_WINDOWS
        void* buffer_base( const WSABUF* buf );
        void* buffer_base( WSABUF* buf , void* ptr );
        std::size_t buffer_length( const WSABUF* buf );
        std::size_t buffer_length( WSABUF* buf , std::size_t len );
#else
        void* buffer_base( const iovec* buf );
        void* buffer_base( iovec* buf , void* ptr );
        std::size_t buffer_length( const iovec* buf );
        std::size_t buffer_length( iovec* buf , std::size_t len );
#endif
    }

    template < typename T > class basic_buffer : public T {
    public:
		basic_buffer(void) {
			detail::buffer_base(raw_buffer(), nullptr);
			detail::buffer_length(raw_buffer(), 0);
		}

        basic_buffer( void* ptr , std::size_t len ) {
            detail::buffer_base( raw_buffer() ,  ptr );
            detail::buffer_length( raw_buffer() ,  len );
        }

        basic_buffer( const std::string_view& msg ) {   
            detail::buffer_base( raw_buffer() ,  const_cast<char*>(msg.data()) );
            detail::buffer_length( raw_buffer() ,  msg.size() );
        }

        void* base( void ) const { return detail::buffer_base( raw_buffer()); }

        std::size_t length( void ) const { return detail::buffer_length( raw_buffer()); }

        void* base( void* new_ptr ) { 
            return detail::buffer_base( raw_buffer() , new_ptr );
        }

        std::size_t length( const std::size_t new_size ) {
            return detail::buffer_length( raw_buffer() , new_size );
        }

        T* raw_buffer( void ) const {
            return static_cast< T* >(const_cast<basic_buffer*>(this));
        }
    };

#if CX_PLATFORM == CX_P_WINDOWS
    using buffer = basic_buffer< WSABUF >;
#else

#endif
}

#endif
