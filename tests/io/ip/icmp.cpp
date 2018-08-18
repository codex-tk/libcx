/**
 */

#include <cx/io/ip/hdr.hpp>
#include <cx/io/basic_engine.hpp>
#include "tests/gprintf.hpp"


TEST( cx_io_ip , hdr ) {

	cx::io::engine<
		cx::io::ip::tcp::service ,
		cx::io::ip::udp::service ,
		cx::time::timer_service ,
		cx::io::ip::icmp::service
	> engine;

	cx::io::ip::icmp::socket fd(engine);
	ASSERT_TRUE(fd.open(cx::io::ip::icmp::address::any(0, AF_INET)));

	cx::io::ip::icmphdr icmphdr;

	cx::io::ip::icmp::buffer wrbuf;
	wrbuf.address = cx::io::ip::icmp::address("192.168.1.1", 0, AF_INET);
	wrbuf.buffer = cx::io::buffer(&icmphdr,sizeof(icmphdr));

	cx::io::ip::icmp::buffer rdbuf;
	char read_buf[1024] = { 0 , };
	rdbuf.buffer = cx::io::buffer(read_buf, 1024);

	icmphdr.icmp_type = 8; //ICMP_ECHO;
	icmphdr.icmp_code = 0;
	icmphdr.icmp_checksum = 0;
	icmphdr.icmp_sequence= 15;
	icmphdr.icmp_id = 81;
	icmphdr.icmp_checksum = cx::checksum(&icmphdr, sizeof(icmphdr));
	
	ASSERT_EQ(fd.write(wrbuf), static_cast<int>(sizeof(icmphdr)));

	fd.async_read(rdbuf , [&](const std::error_code& ec, int sz) {
		if (!ec && sz > 0) {
			cx::io::ip::iphdr* piphdr = reinterpret_cast<cx::io::ip::iphdr*>(read_buf);
			int offset = piphdr->ip_hl * 4;
			if ( sz > offset ) {
				cx::io::ip::icmphdr* picmphdr = reinterpret_cast<cx::io::ip::icmphdr*>(read_buf + offset);
				gprintf("Type : %d \n", picmphdr->icmp_type);
				gprintf("Code : %d \n", picmphdr->icmp_code);
				gprintf("Seq  : %d \n", picmphdr->icmp_sequence);
				gprintf("Iden : %d \n", picmphdr->icmp_id);
			}
		}
		fd.close();
	});
	engine.run();
    
}
