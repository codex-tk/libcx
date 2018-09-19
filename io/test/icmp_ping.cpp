/**
 * @brief 
 * 
 * @file icmp_ping.cpp
 * @author ghtak
 * @date 2018-09-19
 */

#include <gtest/gtest.h>
#include <cx/base/utils.hpp>
#include <cx/io/services.hpp>
#include <cx/io/ip/icmp.hpp>
#include "gprintf.hpp"

TEST(cx_io_ip_icmp, ping){
    cx::io::engine e;
    cx::io::ip::icmp::socket fd(e);
	bool result = fd.open(cx::io::ip::icmp::address::any(0, AF_INET));
#if defined(CX_PLATFORM_WIN32)
    ASSERT_TRUE(result);
#else
	return;
#endif
	
	cx::io::ip::icmp::address to("192.168.1.1", 0, AF_INET);
	cx::io::ip::icmp::icmphdr hdr;

	hdr.icmp_type = cx::io::ip::icmp::type::echo;
	hdr.icmp_code = 0;
	hdr.icmp_checksum = 0;
	hdr.icmp_sequence = 0;
	hdr.icmp_id = 0x81;
	hdr.icmp_checksum = 0;
	hdr.icmp_checksum = cx::checksum(&hdr, sizeof(hdr));

	ASSERT_EQ(fd.sendto(cx::io::buffer(&hdr, sizeof(hdr)), to), sizeof(hdr));

	cx::basic_buffer<uint8_t> buf(1024);
	fd.async_recvfrom(cx::io::rdbuf(buf), [&](const std::error_code& ec
		, const int size
		, const cx::io::ip::icmp::address& )
	{
		ASSERT_FALSE(ec);
		ASSERT_TRUE(size > 0);
		buf.commit(size);
		
		//const char map[] = "01234567890ABCDEF";
		//std::string output;
		//for (int i = 0; i < size; ++i) {
		//	if (i != 0 && i % 4 == 0) output += " ";
		//	uint8_t* ptr = buf.rdptr() + i;
		//	output += map[(*ptr) >> 4];
		//	output += map[*ptr & 0x0f];
		//}
		//gprintf("Recv : %s", output.c_str());
		
		cx::io::ip::iphdr* iphdr = reinterpret_cast<cx::io::ip::iphdr*>(buf.rdptr());
		ASSERT_TRUE(iphdr->length() <= buf.rdsize());
		buf.consume(iphdr->length());
		ASSERT_TRUE(buf.rdsize() >= sizeof(cx::io::ip::icmp::icmphdr));
		cx::io::ip::icmp::icmphdr* icmphdr = reinterpret_cast<cx::io::ip::icmp::icmphdr*>(buf.rdptr());
		//gprintf("icmp echo type(%d) code(%d) seq(%d) id(%d) size(%d) ipver(%d) iplen(%d)",
		//	icmphdr->icmp_type, icmphdr->icmp_code, icmphdr->icmp_sequence, icmphdr->icmp_id,
		//	size, iphdr->ver(), iphdr->length());
		ASSERT_EQ(icmphdr->icmp_type, cx::io::ip::icmp::type::echo_reply);
	});

	int val = 0;
	while (val < 1)
		val += e.run(std::chrono::milliseconds(1000));

	ASSERT_EQ(val, 1);

	fd.close();
}