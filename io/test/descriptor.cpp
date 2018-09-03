/**
 * @brief
 *
 * @file descriptor.cpp
 * @author ghtak
 * @date 2018-08-26
 */
#include <gtest/gtest.h>
#include <cx/io/io.hpp>

template < typename MuxT >
struct event_handler;
#if defined( CX_PLATFORM_WIN32 )
#include <cx/io/mux/completion_port.hpp>

using mux = cx::io::mux::completion_port;

template <>
struct event_handler<cx::io::mux::completion_port> {
	void operator()(cx::io::mux::completion_port::descriptor_ptr& descriptor, int revt) {
		int ctx_idx = revt >> 2;
		if (ctx_idx >= 0 && ctx_idx <= 1) {
			cx::io::mux::completion_port::operation* op = descriptor->context[ctx_idx].ops.head();
			if (op) {
				op->set(std::error_code(), 0);
				if (op->complete(descriptor)) {
					descriptor->context[ctx_idx].ops.remove_head();
					(*op)();
					if (descriptor->context[ctx_idx].ops.head()) {
						descriptor->context[ctx_idx].ops.head()->request(descriptor);
					}
				}
			}
		}
	}
};

#elif defined( CX_PLATFORM_LINUX )

#include <cx/io/mux/epoll.hpp>

using mux = cx::io::mux::epoll;

template <>
struct event_handler<cx::io::mux::epoll> {
	void operator()(cx::io::mux::epoll::descriptor_ptr& descriptor, int revt) {
		int ops_filter[2] = { cx::io::pollin , cx::io::pollout };
		for (int i = 0; i < 2; ++i) {
			if (ops_filter[i] & revt) {
				cx::io::mux::epoll::operation* op = descriptor->context[i].ops.head();
				if (op && op->complete(descriptor)) {
					descriptor->context[i].ops.remove_head();
					(*op)();
				}
			}
		}
	}
};

#endif

namespace cx::testing {
	namespace {
		static int in_op_called = 0;
		static int out_op_called = 0;

		class in_operation : public mux::operation {
		public:
			virtual bool complete(const mux::descriptor_ptr&) override {
				return true;
			}

			virtual void request(const descriptor_ptr&) override {
			}

			virtual void operator()(void) override {
				++in_op_called;
			}
		};

		class out_operation : public mux::operation {
		public:
			virtual bool complete(const mux::descriptor_ptr&) override {
				return true;
			}

			virtual void request(const descriptor_ptr&) override {
			}

			virtual void operator()(void) override {
				++out_op_called;
			}
		};
	}
}



TEST(cx_io, descriptor) {
	//cx::io::engine dummy_engine;

	mux::descriptor_ptr descriptor = std::make_shared<mux::descriptor>();
	// simulate req io
	std::vector< std::shared_ptr<mux::operation >> inops{
		std::make_shared<cx::testing::in_operation>() ,
		std::make_shared<cx::testing::in_operation>() ,
		std::make_shared<cx::testing::in_operation>()
	};

	std::vector< std::shared_ptr<mux::operation >> outops{
		std::make_shared<cx::testing::out_operation>() ,
		std::make_shared<cx::testing::out_operation>() ,
		std::make_shared<cx::testing::out_operation>()
	};

	for (auto op : inops) {
		op->set(std::error_code(), 0);
		descriptor->context[0].ops.add_tail(op.get());
	}

	for (auto op : outops) {
		op->set(std::error_code(), 0);
		descriptor->context[1].ops.add_tail(op.get());
	}

	event_handler<mux> handler;

	handler(descriptor, cx::io::pollin);
	handler(descriptor, cx::io::pollin);
	ASSERT_EQ(cx::testing::in_op_called, 2);

	handler(descriptor, cx::io::pollout);
	handler(descriptor, cx::io::pollout);
	ASSERT_EQ(cx::testing::out_op_called, 2);
#if defined( CX_PLATFORM_WIN32 )
	handler(descriptor, cx::io::pollin);
	handler(descriptor, cx::io::pollout);
#else
	handler(descriptor, cx::io::pollin | cx::io::pollout);
#endif
	ASSERT_EQ(cx::testing::in_op_called, 3);
	ASSERT_EQ(cx::testing::out_op_called, 3);
}