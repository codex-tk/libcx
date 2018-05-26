#ifndef __cx_io_select_h__
#define __cx_io_select_h__

#include <cx/io/io.hpp>

namespace cx::io{

    class selector{
    public:
        template < typename DescriptorT >
        static int select( DescriptorT& fd , int ops , int wait_milli_sec ) {
            fd_set rdfds , wrfds;
            FD_ZERO( &rdfds );
            FD_ZERO( &wrfds );
            fd_set* sets[2] = { &rdfds , &wrfds };
            int io_ops[2] = { io::ops::read , io::ops::write };
            for ( int i = 0 ; i < 2 ; ++i ) {
                if ( ops & io_ops[i] ) {
                    FD_SET( fd.descriptor() , sets[i] );
                }
            }
\
            struct timeval tv;
            tv.tv_sec = wait_milli_sec / 1000;
            tv.tv_usec =( wait_milli_sec % 1000 ) * 1000;
            int ret = ::select( fd.descriptor() + 1 , &rdfds , &wrfds , nullptr  , &tv );
            int result = 0;
            if ( ret > 0  ) {
                for ( int i = 0 ; i < 2 ; ++i ) {
                    if ( FD_ISSET( fd.descriptor() , sets[i] ) ) {
                        result |= io_ops[i];
                    }
                }
            }
            return result;
        }
    };

}

#endif