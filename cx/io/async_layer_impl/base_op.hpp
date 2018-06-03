/**
 */
#ifndef __cx_io_base_op_h__
#define __cx_io_base_op_h__

#include <cx/cxdefine.hpp>

namespace cx::io {

    class base_op{
    public:
        base_op(void) {
#if CX_PLATFORM == CX_P_WINDOWS
            memset(overlapped(), 0x00, sizeof(_ov));
#endif
        }
        
        virtual ~base_op(void) = default;

        int io_size( void ) {
#if CX_PLATFORM == CX_P_WINDOWS
            return _ov.Offset;
#else
            return _io_size;
#endif
        }
        int io_size( int sz ){
#if CX_PLATFORM == CX_P_WINDOWS
            int t = _ov.Offset;
            _ov.Offset = sz;
            sz = t;
#else
            std::swap(sz, _io_size);
#endif
            return sz;
        }
        
        std::error_code error( void ) { 
            return _ec;
        }

        std::error_code error( const std::error_code& ec ) { 
            std::error_code old(_ec);
            _ec = ec;
            return old; 
        }

        virtual void operator()(void) = 0;
    private:
        std::error_code _ec;
#if CX_PLATFORM == CX_P_WINDOWS
    public:
        OVERLAPPED _ov;
#else
        int _io_size;
#endif

#if CX_PLATFORM == CX_P_WINDOWS
    public:
        OVERLAPPED *overlapped(void) {
            return &_ov;
        }
#endif
    };

}

#endif