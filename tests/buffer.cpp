/**
 * @brief 
 * 
 * @file buffer.cpp
 * @author ghtak
 * @date 2018-05-12
 */
#include "gtest/gtest.h"
#include <cx/core/buffer.hpp>

TEST( cx_core_buffer , generate ) {
    cx::basic_buffer< int , int > buf;
	(void*)&buf;
}