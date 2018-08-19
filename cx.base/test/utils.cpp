/**
 * @brief 
 * 
 * @file utils.cpp
 * @author ghtak
 * @date 2018-08-19
 */

#include <gtest/gtest.h>
#include <cx/base/utils.hpp>

TEST(cx_util, checksum){
	int value = 32;
	ASSERT_EQ(cx::base::checksum(&value, sizeof(value)), 65503);
	
	char hello[] = "hello";
	ASSERT_EQ(cx::base::checksum(hello, strlen(hello)),11708);
}