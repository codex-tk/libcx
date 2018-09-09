/**
 * @brief 
 * 
 * @file basic_ops.cpp
 * @author ghtak
 * @date 2018-09-09
 */

#include <gtest/gtest.h>
#include <cx/io/basic_engine.hpp>
#include <cx/io/ops/basic_io_operation.hpp>
#include <cx/io/ops/basic_read_operation.hpp>
#include <cx/io/ip/services/basic_dgram_service.hpp>

namespace cx::testing {
#if defined(CX_PLATFORM_WIN32)
	using mux = cx::io::mux::completion_port;
	using engine = cx::io::basic_engine<cx::io::mux::completion_port>;
#elif defined(CX_PLATFORM_LINUX)
	using mux = cx::io::mux::epoll;
	using engine = cx::io::basic_engine<cx::io::mux::epoll>;
#endif
}

TEST(cx_io, basic_ops) {
	using service_type = cx::io::ip::basic_dgram_service<
		cx::testing::engine>;
	cx::io::basic_read_operation<service_type,
		cx::io::basic_io_operation<service_type>>* op;

	CX_UNUSED(op);
}