/**
 * @brief 
 * 
 * @file engine.cpp
 * @author ghtak
 * @date 2018-08-26
 */

#include <gtest/gtest.h>
#include <cx/io/engine.hpp>

TEST(cx_io_engine,basic) {
    cx::io::engine e;
	cx::io::mux::impl_t impl(e);
}