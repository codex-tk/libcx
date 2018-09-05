/**
 * @brief 
 * 
 * @file engine.cpp
 * @author ghtak
 * @date 2018-08-26
 */

#include <gtest/gtest.h>
#include <cx/io/basic_engine.hpp>

#if defined(CX_PLATFORM_WIN32)
using engine = cx::io::basic_engine<cx::io::mux::completion_port>;
#elif defined(CX_PLATFORM_LINUX)
using engine = cx::io::basic_engine<cx::io::mux::epoll>;
#endif

namespace cx::testing {

	template < typename HandlerT >
	class _op : public engine::operation_type {
	public:
		_op(HandlerT&& h)
			: _handler(std::forward<HandlerT>(h)) {

		}
		virtual bool complete(const engine::descriptor_type&) override {
			return true;
		}

		virtual void request(const engine::descriptor_type&) override {
		}

		virtual void operator()(void) override {
			_handler();
			delete this;
		}
	private:
		HandlerT _handler;
	};

	template < typename HandlerT >
	static engine::operation_type* make_op(HandlerT&& h) {
		return new _op<HandlerT>(std::forward<HandlerT>(h));
	}
}

TEST(cx_io_engine, basic) {
	engine engine;
	int test_value = 0;
	engine.post(cx::testing::make_op([&] {
		test_value = 1;
	}));
	engine.run(std::chrono::milliseconds(1));
	ASSERT_EQ(test_value, 1);
}

