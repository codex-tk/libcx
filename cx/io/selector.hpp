#ifndef __cx_io_select_h__
#define __cx_io_select_h__

#include <cx/io/io.hpp>
#include <array>
namespace cx::io{

    template < typename HandleT , std::size_t N >
    class selector {
    public:
        using handle_type = typename cx::io::handle_trait<HandleT>::handle_type;
        using context = std::tuple< handle_type , int , void* >;
        enum context_id {
            handle = 0 ,
            ops = 1 , 
            udata = 2 ,
        };

        selector( void ) : _fds_length(0) , _sigfds_length(0) {
#if CX_PLATFORM == CX_P_WINDOWS
            static_assert( N < 64 );
#endif
            for ( std::size_t i = 0 ; i < N ; ++i ) {
                std::get<context_id::handle>(_fds[i]) = cx::io::handle_trait<HandleT>::invalid();
                std::get<context_id::handle>(_sigfds[i]) = cx::io::handle_trait<HandleT>::invalid();
            }            
        }

        ~selector( void ){
        }

        bool bind( handle_type fd , int ops , void* udata ) {
            for ( std::size_t i = 0 ; i < N ; ++i ) {
                if ( std::get<context_id::handle>(_fds[i]) == cx::io::handle_trait<HandleT>::invalid() ) {
                    std::get<context_id::handle>(_fds[i]) = fd;
                    std::get<context_id::ops>(_fds[i]) = ops;
                    std::get<context_id::udata>(_fds[i]) = udata;
                    ++_fds_length;
                    return true;
                }
                if ( std::get< context_id::handle >( _fds[i] ) == fd ) {
                    std::get<context_id::ops>(_fds[i]) = ops;
                    std::get<context_id::udata>(_fds[i]) = udata;
                    return true;
                }
            }
            return false;
        }

        void unbind( handle_type fd ) {
            for ( std::size_t i = 0 ; i < _fds_length ; ++i ) {
                if ( std::get< context_id::handle >( _fds[i]) == fd ) {
                    if ( i == _fds_length - 1 ) {
                        std::get<context_id::handle>(_fds[i]) = cx::io::handle_trait<HandleT>::invalid();
                    } else {
                        std::get<context_id::handle>(_fds[i]) = std::get<context_id::handle>(_fds[_fds_length -1]);
                        std::get<context_id::ops>(_fds[i]) = std::get<context_id::ops>(_fds[_fds_length -1]);
                        std::get<context_id::udata>(_fds[i]) = std::get<context_id::udata>(_fds[_fds_length -1]);
                        std::get<context_id::handle>(_fds[_fds_length -1]) = cx::io::handle_trait<HandleT>::invalid();
                    }
                }
            }
        }

        int select( int wait_milli_sec ) {
            _sigfds_length = 0;
            fd_set rdfds;
            fd_set wrfds;
            FD_ZERO( &rdfds );
            FD_ZERO( &wrfds );
#if CX_PLATFORM != CX_P_WINDOWS
            int maxfd = 0;
#endif
            for ( std::size_t i = 0 ; i < _fds_length ; ++i ) {
                handle_type fd = std::get<context_id::handle>(_fds[i]);
                int ops = std::get< context_id::ops >(_fds[i]);
#if CX_PLATFORM != CX_P_WINDOWS
                if ( fd > maxfd ) maxfd = std::get<context_id::handle>(_fds[i]);
#endif
                if ( ops & cx::io::pollin )
                    FD_SET( fd , &rdfds );
                if ( ops & cx::io::pollout )
                    FD_SET( fd , &wrfds );
            }    

            struct timeval tv;
            tv.tv_sec = wait_milli_sec / 1000;
            tv.tv_usec =( wait_milli_sec % 1000 ) * 1000;

            int ret = 0;
#if CX_PLATFORM == CX_P_WINDOWS
            ret = ::select( 0 , &rdfds , &wrfds , nullptr  , &tv );
            if ( ret == 0 || ret == -1 ) // timeout or error
                return 0;
#else
            while ( true ) {
                ret = ::select( maxfd  + 1 , &rdfds , &wrfds , nullptr  , &tv );
                if ( ret > 0 )  
                    break;
                if ( ret == -1 && errno == EINTR ) 
                    continue;
                return 0;
            }
#endif
            for ( std::size_t i = 0 ; i < _fds_length ; ++i ) {
                bool pushed = false;
                handle_type fd = std::get<context_id::handle>(_fds[i]);
                int ops = std::get< context_id::ops >(_fds[i]);
                if ( ops & cx::io::pollin ) {
                    if ( FD_ISSET( fd , &rdfds )) {
                        std::get<context_id::handle>(_sigfds[_sigfds_length]) = fd;
                        std::get<context_id::ops>(_sigfds[_sigfds_length]) = cx::io::pollin;
                        pushed = true;
                    }
                }
                if ( ops & cx::io::pollout ) {
                    if ( FD_ISSET( fd , &wrfds )) {
                        if ( pushed ) {
                            std::get<context_id::ops>(_sigfds[_sigfds_length]) |= cx::io::pollin;
                        } else {
                            std::get<context_id::handle>(_sigfds[_sigfds_length]) = fd;
                            std::get<context_id::ops>(_sigfds[_sigfds_length]) = cx::io::pollout;
                            pushed = true;
                        }                 
                    }
                }
                if ( pushed ) ++_sigfds_length;
            }  
            return ret;
        }
    public:
        class iterator {
        public:
            iterator ( selector& s ) : _selector(s) , _index(0) {}
            iterator ( selector& s , std::size_t index ) : _selector(s) , _index(index) {}

            handle_type handle( void ) { 
                return std::get< context_id::handle>( _selector._sigfds[_index] );
            }
            int ops( void ) {  
                return std::get< context_id::ops>(_selector._sigfds[_index]); 
            }
            void* udata( void ) { 
                return std::get< context_id::udata>(_selector._sigfds[_index]); 
            }
            explicit operator bool( void ) const {
                return _index != _selector._sigfds_length;
            }
            iterator& operator++(void) {
                ++_index;
                return *this;
            }
            iterator operator++(int){
                iterator ret( _selector , _index );
                ++_index;
                return ret;
            }
        private:
            selector& _selector;
            std::size_t _index;
        };
    private:
        std::array< context , N > _fds;
        std::size_t _fds_length;
        std::array< context , N > _sigfds;
        std::size_t _sigfds_length;
    public:
        static int select( handle_type fd , int ops , int wait_milli_sec ) {
            selector s;
            s.bind( fd , ops , nullptr );
            if (  s.select( wait_milli_sec) > 0 ) {
                selector::iterator it( s );
                ops = it.ops();
            } else {
                ops = 0;
            }
            return ops;
        }
    };

}

#endif
