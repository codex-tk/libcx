/**
 * @brief 
 * 
 * @file completion_port.cpp
 * @author ghtak
 * @date 2018-08-26
 */

#include <gtest/gtest.h>
#include <cx/io/engine.hpp>
#include <cx/io/internal/completion_port.hpp>

#if defined(CX_PLATFORM_WIN32)

TEST(cx_io, completion_port) {
	cx::io::engine e;
	cx::io::internal::completion_port port(e);
}


#endif