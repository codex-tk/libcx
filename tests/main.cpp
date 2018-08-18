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
	cx::log::core::instance()->add_sink(sink);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}