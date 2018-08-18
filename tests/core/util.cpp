#include "gtest/gtest.h"
#include "tests/gprintf.hpp"
#include <cx/core/util.hpp>

#include <cx/log/log.hpp>
#include <cx/log/core.hpp>


TEST(cx_util, checksum)
{
	int value = 32;
	ASSERT_EQ(cx::checksum(&value, sizeof(value)), 65503);
	
	char hello[] = "hello";
	ASSERT_EQ(cx::checksum(hello, strlen(hello)),11708);
}