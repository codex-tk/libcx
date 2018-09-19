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
#pragma pack(push,1)

	enum type : uint8_t {
		echo_reply = 0,
		destination_unreachable = 3,
		source_quench = 4,
		redirect = 5,
		echo = 8,
		router_advertisement = 9,
		router_selection = 10,
		time_exceeded = 11,
		parameter_problem = 12,
		timestamp = 13,
		timestamp_reply = 14,
		information_request = 15,
		information_reply = 16,
		address_mask_request = 17,
		address_mask_reply = 18,
		traceroute = 30,
	};

	struct icmphdr {
		uint8_t   icmp_type;
		uint8_t   icmp_code;
		uint16_t  icmp_checksum;
		uint16_t  icmp_id;
		uint16_t  icmp_sequence;
	};

#pragma pack(pop)
}

#endif