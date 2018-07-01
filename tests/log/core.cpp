#include "tests/gprintf.hpp"
#include <cx/log/core.hpp>

TEST( log , core ) {
    auto sink = std::make_shared<cx::log::sink>(
				cx::log::filter::default_filter() ,
				cx::log::string_formatter::instance()
			);
    sink->add_writer( cx::log::cout_writer::instance() );
	cx::log::core::instance()->add_sink(sink);
	CXLOG(cx::log::level::debug, "Sample Tag", "Test Message %d %s", 10, "StringMessage");
	char dump[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	CXDUMP(cx::log::level::info, "Tag", dump , 32 , "Test Dump %d %s", 10, "StringMessage");
	CXLOG_D("Test", "Debug %d %s %d", 1, "!!", 2);
}