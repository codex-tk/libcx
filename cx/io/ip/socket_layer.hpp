/**
 * @brief 
 * 
 * @file socket_layer.hpp
 * @author ghtak
 * @date 2018-05-31
 */
#include <cx/io/ip/basic_address.hpp>

namespace cx::io::ip {
    
    template < int Type , int Proto >
    class socket_layer;


    template <>
    class socket_layer< SOCK_STREAM , IPPROTO_TCP >{
    public:
        using buf_type = cx::io::basic_buffer;
        using bufv_type = std::vector<cx::io::basic_buffer>;

        static socket_type open( int family ) {
  #if CX_PLATFORM == CX_P_WINDOWS          
            return ::WSASocketW( family , SOCK_STREAM , IPPROTO_TCP , nullptr , 0 , WSA_FLAG_OVERLAPPED  );
  #else
            return ::socket( family , SOCK_STREAM , IPPROTO_TCP ); 
  #endif
        }

        static void close( socket_type fd ) {
  #if CX_PLATFORM == CX_P_WINDOWS       
            ::closesocket( fd );
  #else
            ::close(fd);
  #endif            
        }

        static bool connect( socket_type fd , const cx::io::ip::address& address ) {
            if (::connect( fd , address.sockaddr() , address.length()) == 0 )
                return true;
#if CX_PLATFORM == CX_P_WINDOWS
            if ( WSAGetLastError() == WSAEWOULDBLOCK ) 
                return true;
#else
            if ( errno == EINPROGRESS )
                return true;
#endif
            return false;
        }

        static bool listen( socket_type fd ) {
            return ::listen( fd , SOMAXCONN ) != -1;
        }
        
        static socket_type accept( socket_type fd , cx::io::ip::address& addr ) {
            return ::accept( fd , addr.sockaddr() , addr.length_ptr());
        }

        static int write( socket_type fd ,const buf_type& buf ) {
            return send( fd , static_cast<const char*>(buf.ptr()) , buf.len() , 0 );
        }

        static int read( socket_type fd ,const buf_type& buf ) {
            return recv( fd , static_cast<char*>(buf.ptr()) , buf.len() , 0 );
        }

        static int writev( socket_type fd , const bufv_type& buf ) {
            return 0;
        }

        static int readv( socket_type fd , const bufv_type& buf ) {
            return 0;
        }
    };
    
}