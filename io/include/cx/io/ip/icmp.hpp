/**
 * @brief
 *
 * @file icmp.hpp
 * @author ghtak
 * @date 2018-09-19
 */

#ifndef __cx_io_ip_icmp_h__
#define __cx_io_ip_icmp_h__

#include <cx/base/defines.hpp>

namespace cx::io::ip::icmp {

	struct icmphdr {
		uint8_t   icmp_type;
		uint8_t   icmp_code;
		uint16_t  icmp_checksum;
		uint16_t  icmp_id;
		uint16_t  icmp_sequence;
	};

}

#endif