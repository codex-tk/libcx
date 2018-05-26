#include <cx/cx.hpp>
#include <gtest/gtest.h>

TEST( cx , hello ) {
    std::string hello = cx::say_hello();
    std::string s = "À";
    std::cout << s.length() << std::endl;
    std::size_t len = s.length();
	ASSERT_EQ(hello, "hello cx");
    ASSERT_NE(hello, "bye cx");
}