#ifndef __cx_io_select_h__
#define __cx_io_select_h__

#include <cx/io/io.hpp>

namespace cx::io{

    class selector{
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