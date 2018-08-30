/**
 * @brief 
 * 
 * @file epoll.cpp
 * @author ghtak
 * @date 2018-08-26
 */

#include <gtest/gtest.h>
#include <cx/io/engine.hpp>
#include <cx/io/mux/epoll.hpp>

#if defined(CX_PLATFORM_LINUX)

TEST(cx_io, epoll) {
	cx::io::engine e;
	cx::io::mux::epoll port(e);
}


#endif