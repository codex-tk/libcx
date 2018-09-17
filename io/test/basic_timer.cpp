/**
 * @brief 
 * 
 * @file basic_timer.cpp
 * @author ghtak
 * @date 2018-09-17
 */

#include <gtest/gtest.h>
#include "gprintf.hpp"
#include <cx/io/services.hpp>

TEST(cx_timer, basic_timer) {
	cx::io::engine e;
	cx::timer::timer timer(e);
	timer.schedule(
		std::chrono::system_clock::now() + std::chrono::milliseconds(100), 
		[](const std::error_code&) {

		});

	ASSERT_EQ(e.run(std::chrono::seconds(1000)), 1);
}