#include <cx/cx.hpp>
#include <gtest/gtest.h>
#include "tests/gprintf.hpp"

void gprintf( const char* fmt , ... ) {
    printf( "[   USER   ] " );
    va_list args;
    va_start(args, fmt);
    vprintf( fmt , args );
    va_end(args);
    printf( "\n" );
}


TEST( cx , hello ) {
    std::string hello = cx::say_hello();
    //std::string s = "À";
    //std::cout << s.length() << std::endl;
    //std::size_t len = s.length();
	ASSERT_EQ(hello, "hello cx");
    ASSERT_NE(hello, "bye cx");
}