/**
 * @brief
 *
 * @file operation.cpp
 * @author ghtak
 * @date 2018-08-26
 */
#include <gtest/gtest.h>
/*
#include <cx/io/operation.hpp>


namespace cx::testing {
	namespace {
		class operation : public cx::io::operation {
		public:
			virtual bool is_complete(const cx::io::descriptor_type&) override {
				return true;
			}

			virtual void operator()(void) override {

			}
		};
	}
}

TEST(cx_io, operation) {
	cx::testing::operation op;

	op.set(std::make_error_code(std::errc::address_in_use), 2);

	ASSERT_EQ(op.error(), std::make_error_code(std::errc::address_in_use));
	ASSERT_EQ(op.size(), 2);

#if defined(CX_PLATFORM_WIN32)
	ASSERT_EQ(op.type(), 0);
	OVERLAPPED* ov = op.reset_overlapped(cx::io::pollin);
	ASSERT_EQ(op.type(), cx::io::pollin);
#endif

}*/