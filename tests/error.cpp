#include <gtest/gtest.h>
#include <cx/cxdefine.hpp>
#include <cx/error.hpp>

TEST(error, t0) {
	std::error_code ec;
	ASSERT_FALSE(ec == cx::errc::success);

    ASSERT_FALSE(cx::make_error_code(cx::errc::success));

	ec = cx::make_error_code(cx::errc::success);
	ASSERT_FALSE(ec == std::errc::operation_canceled);

	ec = cx::make_error_code(cx::errc::operation_canceled);
	ASSERT_TRUE(ec == std::errc::operation_canceled);
	ASSERT_TRUE(std::errc::operation_canceled == ec);

}
