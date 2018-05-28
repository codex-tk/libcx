#ifndef __cx_io_select_h__
#define __cx_io_select_h__

#include <cx/io/io.hpp>
#include <map>
namespace cx::io{

    class selector{
    public:
        using container_type = std::map< 
            cx::io::descriptor_type 
            , std::tuple< int , void* > >;
        selector( void ){

        }

        ~selector( void ){

        }

        bool bind( cx::io::descriptor_type fd , int ops , void* ctx ) {
            _fds[fd] = std::make_tuple( ops , ctx );
            return true;
        }

        void unbind( cx::io::descriptor_type fd ) {
            _fds.erase( fd );
        }

        int select( int wait_milli_sec ) {
            _sigfds.clear();
            fd_set rdfds;
            fd_set wrfds;
            fd_set* sets[2] = { &rdfds , &wrfds };
            int io_ops[2] = { io::ops::read , io::ops::write };
            for ( int i = 0 ; i < 2 ; ++i ) {
                FD_ZERO( sets[i] );
            }
            for ( auto fd : _fds ){
                for ( int i = 0 ; i < 2 ; ++i ) {
                    if ( std::get<0>(fd.second) & io_ops[i] )
                        FD_SET( fd.first , sets[i] );
                }
            }    

            struct timeval tv;
            tv.tv_sec = wait_milli_sec / 1000;
            tv.tv_usec =( wait_milli_sec % 1000 ) * 1000;

#if CX_PLATFORM == CX_P_WINDOWS
            int ret = ::select( 0 , &rdfds , &wrfds , nullptr  , &tv );
            if ( ret == 0 || ret == -1 ) // timeout or error
                return 0;
#else
            while ( true ) {
                int ret = ::select( fd.descriptor() + 1 , &rdfds , &wrfds , nullptr  , &tv );
                if ( ret > 0 )  
                    break;
                if ( ret == -1 && errno == EINTR ) 
                    continue;
                return 0;
            }
#endif
            for ( auto fd : _fds ){
                for ( int i = 0 ; i < 2 ; ++i ) {
                    if ( (std::get<0>(fd.second) & io_ops[i]) == 0 ) 
                        continue;
                    if (!FD_ISSET( fd.first , sets[i] )) 
                        continue;
                    if ( _sigfds.find( fd.first ) == _sigfds.end() ) {
                        _sigfds[fd.first] = std::make_tuple( io_ops[i] , std::get<1>(fd.second) );
                    } else {
                        std::get<0>(_sigfds[fd.first]) |= io_ops[i];
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
            bool has_next( void ) {  return _it != _selector._sigfds.end(); }
            void next( void ) { ++_it; }
            cx::io::descriptor_type descriptor( void ) { return _it->first; }
            int signal( void ) { return std::get<0>(_it->second); }
            void* context( void ) { return std::get<1>(_it->second); }
        private:
            selector& _selector;
            selector::container_type::iterator _it;
        };
    private:
        container_type _fds;
        container_type _sigfds;
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