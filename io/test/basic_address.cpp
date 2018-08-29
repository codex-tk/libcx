/**
 * @brief
 *
 * @file basic_address.cpp
 * @author ghtak
 * @date 2018-08-27
 */
#include <gtest/gtest.h>
#include <cx/io/ip/basic_address.hpp>
#include <cstdarg>

void gprintf(const char* fmt, ...) {
	printf("[   USER   ] ");
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("\n");
}


namespace tcp {
	using address = cx::io::ip::basic_address<SOCK_STREAM, IPPROTO_TCP>;
}
namespace udp {
	using address = cx::io::ip::basic_address<SOCK_DGRAM, IPPROTO_UDP>;
}


TEST(cx_io_ip, basic_address) {
	auto ipv4 = tcp::address::anys(80, AF_INET);
	auto ipv6 = tcp::address::anys(80, AF_INET6);
	auto all = tcp::address::anys(80, AF_UNSPEC);
	std::for_each(ipv4.begin(), ipv4.end(), [](auto a) { gprintf("V4 %s", a.to_string().c_str()); });
	std::for_each(ipv6.begin(), ipv6.end(), [](auto a) { gprintf("V6 %s", a.to_string().c_str()); });
	std::for_each(all.begin(), all.end(), [](auto a) { gprintf("ALL %s", a.to_string().c_str()); });

	tcp::address addr("127.0.0.1", 7543 , AF_INET);
	gprintf("%s", addr.to_string().c_str());

	auto addresses = tcp::address::resolve("naver.com", 80, AF_INET);
	for (auto a : addresses) {
		gprintf(a.to_string().c_str());
	}
}