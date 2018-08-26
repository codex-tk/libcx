/**
 * @brief 
 * 
 * @file descriptor.cpp
 * @author ghtak
 * @date 2018-08-26
 */
#include <gtest/gtest.h>
#include <cx/io/io.hpp>
#include <cx/io/descriptor.hpp>
#include <cx/io/operation.hpp>

#include <cx/io/engine.hpp>

namespace cx::testing {
	namespace {
		static int in_op_called = 0;
		static int out_op_called = 0;

		class in_operation : public cx::io::operation {
		public:
			virtual bool is_complete(const cx::io::descriptor_t&) override {
				return true;
			}

			virtual void operator()(void) override {
				++in_op_called;
			}
		};

		class out_operation : public cx::io::operation {
		public:
			virtual bool is_complete(const cx::io::descriptor_t&) override {
				return true;
			}

			virtual void operator()(void) override {
				++out_op_called;
			}
		};
	}
}

TEST(cx_io,descriptor){
	cx::io::engine dummy_engine;

    cx::io::descriptor_t descriptor = std::make_shared<cx::io::descriptor>();
    // simulate req io
	std::vector< std::shared_ptr<cx::io::operation >> inops{
		std::make_shared<cx::testing::in_operation>() ,
		std::make_shared<cx::testing::in_operation>() ,
		std::make_shared<cx::testing::in_operation>()
	};

	std::vector< std::shared_ptr<cx::io::operation >> outops{
		std::make_shared<cx::testing::out_operation>() ,
		std::make_shared<cx::testing::out_operation>() ,
		std::make_shared<cx::testing::out_operation>()
	};

	for (auto op : inops) {
		op->set(std::error_code(), 0);
		descriptor->put(cx::io::pollin, op.get());
	}

	for (auto op : outops) {
		op->set(std::error_code(), 0);
		descriptor->put(cx::io::pollout, op.get());
	}
    // simulate handle_event
	ASSERT_EQ(descriptor->handle_event(dummy_engine, cx::io::pollin), cx::io::pollin);
	ASSERT_EQ(descriptor->handle_event(dummy_engine, cx::io::pollin), cx::io::pollin);
	ASSERT_EQ(cx::testing::in_op_called, 2);

	ASSERT_EQ(descriptor->handle_event(dummy_engine, cx::io::pollout), cx::io::pollout);
	ASSERT_EQ(descriptor->handle_event(dummy_engine, cx::io::pollout), cx::io::pollout);
	ASSERT_EQ(cx::testing::out_op_called, 2);

	ASSERT_EQ(descriptor->handle_event(dummy_engine, cx::io::pollin | cx::io::pollout), cx::io::pollin | cx::io::pollout );
	ASSERT_EQ(cx::testing::in_op_called, 3);
	ASSERT_EQ(cx::testing::out_op_called, 3);

	ASSERT_EQ(descriptor->handle_event(dummy_engine, cx::io::pollin | cx::io::pollout),0);
}