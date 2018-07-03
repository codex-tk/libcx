/**
 * @brief
 *
 * @file main.cpp
 * @author ghtak
 * @date 2018-05-12
 */
#include <gtest/gtest.h>
#include <cx/log/core.hpp>

int main(int argc, char** argv) {
	auto sink = std::make_shared<cx::log::sink>();
	sink->add_writer(cx::log::cout_writer::instance());
#if CX_PLATFORM == CX_P_WINDOWS
	sink->add_writer(cx::log::win32_debug_writer::instance());
#endif
	cx::log::core::instance()->add_sink(sink);
	::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
}