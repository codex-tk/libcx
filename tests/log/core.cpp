#include "tests/gprintf.hpp"
#include <cx/log/core.hpp>

TEST(log, core) {
	auto sink = std::make_shared<cx::log::sink>();
	sink->add_writer(cx::log::cout_writer::instance());
	cx::log::core::instance()->add_sink(sink);
	CXLOG(cx::log::level::debug, "Sample Tag", "Test Message %d %s", 10, "StringMessage");
	char dump[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	CXDUMP(cx::log::level::info, "Tag", dump, 32, "Test Dump %d %s", 10, "StringMessage");
	CXLOG_D("Test", "Debug %d %s %d", 1, "!!", 2);
}

TEST(log, lib_source) {
	auto sink = std::make_shared<cx::log::sink>(
		std::make_shared<cx::log::source_filter<cx::log::lib_source>>());
	sink->add_writer(cx::log::cout_writer::instance());
	cx::log::core::instance()->add_sink(sink);
	CXLOG_D(cx::log::lib_source::instance(), "Lib Message");
	CXLOG_D("CX", "Tag Message");
}