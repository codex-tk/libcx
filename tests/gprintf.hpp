#ifndef __gprintf_h__
#define __gprintf_h__ 

#include <gtest/gtest.h>
#include <stdarg.h>


template < class ...Args >
void gprintf( const char* fmt , Args&&... arg ) {
    printf( "[   USER   ] " );
    va_list args;
    va_start(args, fmt);
    vprintf( fmt , args );
    va_end(args);
    printf( "\n" );
}

#endif