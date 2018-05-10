#include <cx/cx.hpp>
#include <gtest/gtest.h>

TEST( cx , hello ) {
    std::string hello = cx::say_hello();
	ASSERT_EQ(hello, "hello cx");
}