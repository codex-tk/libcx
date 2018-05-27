/**
 * @brief 
 * 
 * @file address.hpp
 * @author ghtak
 * @date 2018-05-26
 */
#ifndef __cx_io_ip_address_h__
#define __cx_io_ip_address_h__

#include <cx/io/io.hpp>
#include <vector>

namespace cx::io::ip{
    namespace detail{
        static bool inet_ntop( const struct sockaddr_storage& addr , char* out , const int len ) {
            void* ptr = nullptr;
            struct sockaddr* psockaddr = reinterpret_cast< struct sockaddr* >(
                                        const_cast<struct sockaddr_storage*>(&addr));
            switch( psockaddr->sa_family ) {
                case AF_INET: ptr = &(reinterpret_cast< sockaddr_in* >(psockaddr)->sin_addr); break;
                case AF_INET6: ptr = &(reinterpret_cast< sockaddr_in6* >(psockaddr)->sin6_addr); break;
            }
            if ( ptr ) {
                return ::inet_ntop( psockaddr->sa_family , ptr , out , len ) != nullptr;
            }
            return false;
        }

    #if CX_PLATFORM != CX_P_WINDOWS
        static bool inet_ntop( const struct sockaddr_un& addr , char* out , const int len ) {
            strncpy( out , addr.sun_path , len );
            return true;
        }
    #endif
    }

    template < typename SockAddrT >
    class basic_address {
    public:
        basic_address( void ) : _length(sizeof(SockAddrT)) {
            memset( &_address , 0x00 , sizeof( _address ));
        }

        basic_address( struct sockaddr* addr_ptr , const int length )
            : _length(length) {
            memcpy( sockaddr()  , addr_ptr , _length );
        }

        basic_address( const basic_address& rhs ) : _length(rhs.length()) {
            memcpy( sockaddr()  , rhs.sockaddr() , _length );
        }

        basic_address( const short family , const char* ip , const uint16_t port ){
            void* ptr = nullptr;
            sockaddr()->sa_family = family;
            if ( family == AF_INET ) {
                struct sockaddr_in* paddr = reinterpret_cast< struct sockaddr_in* >(sockaddr());
                paddr->sin_port = htons(port);  
                ptr = &(paddr->sin_addr);
                _length = sizeof(struct sockaddr_in);
            }
            if ( family == AF_INET6 ) {
                struct sockaddr_in6* paddr = reinterpret_cast< struct sockaddr_in6* >(sockaddr());
                paddr->sin6_port = htons(port);  
                ptr = &(paddr->sin6_addr);
                _length = sizeof(struct sockaddr_in6);
            }
            ::inet_pton(  family , ip , ptr );
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

        bool inet_ntop( char* out , const int len ) const {
            return detail::inet_ntop( _address , out , len );
        }

        const uint16_t family( void ) const {
            return sockaddr()->sa_family;
        }

        int port( void ) const {
            switch( family() ){
                case AF_INET: return ntohs(reinterpret_cast< const struct sockaddr_in* >(sockaddr())->sin_port);
                case AF_INET6: return ntohs(reinterpret_cast< const struct sockaddr_in6* >(sockaddr())->sin6_port);
            }
            return 0;
        }

        std::string to_string( void ) const {
            char buf[MAX_PATH] = { 0 , };
            int len = 0;
            switch( family() ){
                case AF_INET: len += sprintf_s( buf + len , MAX_PATH - len , "AF_INET "); break;
                case AF_INET6: len += sprintf_s( buf + len , MAX_PATH - len , "AF_INET6 "); break;
                default:
                    len += sprintf_s( buf + len ,  MAX_PATH - len , "UNKNOWN "); break;
            }
            if ( this->inet_ntop( buf + len , MAX_PATH - len )){
                len = strlen(buf);
                sprintf_s( buf + len , MAX_PATH - len , " (%d)" , port() );
                return std::string(buf);
            }
            return std::string("");
        }
    public:
    /*
        static basic_address any( const uint16_t port , const short family = AF_INET ) {
            sockaddr_in addr;
            memset( &addr , 0x00 , sizeof(addr));
            addr.sin_family = family; 
            addr.sin_port = htons( port ); 
            addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
            return address( reinterpret_cast< struct sockaddr*>(&addr)
                , sizeof(addr));
        }
    */
        static std::vector< basic_address > resolve( const char* name  
                , const uint16_t port 
                , const uint16_t family = AF_UNSPEC ) {
            struct addrinfo hints;
            struct addrinfo* result = nullptr;
            struct addrinfo* rp = nullptr;

            memset( &hints , 0x00 , sizeof( hints ));

            hints.ai_flags = AI_PASSIVE;
            hints.ai_family = family;
            hints.ai_socktype = SOCK_STREAM;

            char port_str[32] = { 0 , };

            sprintf_s( port_str , "%d" , port );

            std::vector< address > addrs;
            if ( getaddrinfo( name , port_str , &hints , &result ) != 0 )
                return addrs;

            for ( rp = result ; rp != nullptr ; rp = rp->ai_next ){
                basic_address addr( rp->ai_addr , (int)(rp->ai_addrlen));
                addrs.push_back( addr );
            }
            freeaddrinfo( result );
            return addrs;
        }
        static std::vector< basic_address > any( const uint16_t port 
                , const short family = AF_UNSPEC ) {
            return resolve( nullptr , port , family );
        }
    private:
        SockAddrT _address;
        int _length;
    };

    using address = basic_address< struct sockaddr_storage >;
    
}

#endif