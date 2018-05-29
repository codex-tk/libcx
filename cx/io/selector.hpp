#ifndef __cx_io_select_h__
#define __cx_io_select_h__

#include <cx/io/io.hpp>
#include <map>
namespace cx::io{

    class selector{
    public:
        using context = std::tuple< cx::io::descriptor_type , int , void* >;
        enum context_id {
            descriptor = 0 ,
            operations = 1 , 
            user_context = 2 ,
        };
        using container_type = std::map< cx::io::descriptor_type , context >;

        selector( void ){
        }

        ~selector( void ){
        }

        bool bind( cx::io::descriptor_type fd , int ops , void* ctx ) {
            _fds[fd] = std::make_tuple( fd , ops , ctx );
            return true;
        }

        void unbind( cx::io::descriptor_type fd ) {
            _fds.erase( fd );
        }

        int select( int wait_milli_sec ) {
            _sigfds.clear();
            fd_set rdfds;
            fd_set wrfds;
            FD_ZERO( &rdfds );
            FD_ZERO( &wrfds );
#if CX_PLATFORM != CX_P_WINDOWS
            int maxfd = 0;
#endif
            for ( auto fd : _fds ){
#if CX_PLATFORM != CX_P_WINDOWS
                if ( fd.first > maxfd ) maxfd = fd.first;
#endif
                if ( std::get<context_id::operations>(fd.second) & cx::io::ops::read )
                    FD_SET( fd.first , &rdfds );
                if ( std::get<context_id::operations>(fd.second) & cx::io::ops::write )
                    FD_SET( fd.first , &wrfds );
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
            for ( auto fd : _fds ){
                bool pushed = false;
                if ( std::get<context_id::operations>(fd.second) & cx::io::ops::read ) {
                    if ( FD_ISSET( fd.first , &rdfds )) {
                        _sigfds.push_back( fd.second );
                        std::get<context_id::operations>(_sigfds.back()) = cx::io::ops::read;
                        pushed = true;
                    }
                }
                if ( std::get<context_id::operations>(fd.second) & cx::io::ops::write ) {
                    if ( FD_ISSET( fd.first , &wrfds )) {
                        if ( pushed ) {
                            std::get<context_id::operations>(_sigfds.back()) |= cx::io::ops::read;
                        } else {
                            _sigfds.push_back( fd.second );
                            std::get<context_id::operations>(_sigfds.back()) = cx::io::ops::write;
                        }                 
                    }
                }
            }  
            return ret;
        }
    public:
        class iterator {
        public:
            iterator ( selector& s ) : _selector(s) { 
                _it = _selector._sigfds.begin();
            }
            iterator ( selector& s , std::vector< selector::context >::iterator it ) 
                : _selector(s) , _it(it) {
            }
            cx::io::descriptor_type descriptor( void ) { 
                return std::get< context_id::descriptor>(*_it);
            }
            int operations( void ) {  
                return std::get< context_id::operations>(*_it); 
            }
            void* user_context( void ) { 
                return std::get< context_id::user_context>(*_it); 
            }
            explicit operator bool( void ) const {
                return _it != _selector._sigfds.end();
            }
            iterator& operator++(void) {
                ++_it;
                return *this;
            }
            iterator operator++(int){
                iterator ret( _selector , _it );
                ++_it;
                return ret;
            }
        private:
            selector& _selector;
            std::vector< selector::context >::iterator _it;
        };
    private:
        container_type _fds;
        std::vector< context > _sigfds;
    public:
        template < typename T >
        static int select( T& fd , int ops , int wait_milli_sec ) {
            fd_set rdfds;
            fd_set wrfds;
            fd_set* sets[2] = { &rdfds , &wrfds };
            int io_ops[2] = { io::ops::read , io::ops::write };
            for ( int i = 0 ; i < 2 ; ++i ) {
                FD_ZERO( sets[i] );
                if ( ops & io_ops[i] ) {
                    FD_SET( fd.descriptor() , sets[i] );
                }
            }

            struct timeval tv;
            tv.tv_sec = wait_milli_sec / 1000;
            tv.tv_usec =( wait_milli_sec % 1000 ) * 1000;

            int interest_ops = 0;
#if CX_PLATFORM == CX_P_WINDOWS
            int ret = ::select( 0 , &rdfds , &wrfds , nullptr  , &tv );
            if ( ret == 0 || ret == -1 ) // timeout or error
                return interest_ops;
#else
            while ( true ) {
                int ret = ::select( fd.descriptor() + 1 , &rdfds , &wrfds , nullptr  , &tv );
                if ( ret > 0 )  
                    break;
                if ( ret == -1 && errno == EINTR ) 
                    continue;
                return interest_ops;
            }
#endif
            for ( int i = 0 ; i < 2 ; ++i ) {
                if ( FD_ISSET( fd.descriptor() , sets[i] ) ) {
                    interest_ops |= io_ops[i];
                }
            }
            return interest_ops;
        }
    };

}

#endif
