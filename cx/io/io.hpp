#ifndef __cx_io_h__
#define __cx_io_h__

namespace cx::io {

    namespace ops {
        enum {
            read = 0x01 ,
            write = 0x02 ,
            all = 0x03 ,
        };
    }
    
}

#endif