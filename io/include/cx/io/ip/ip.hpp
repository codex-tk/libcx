/**
 * @brief
 *
 * @file ip.hpp
 * @author ghtak
 * @date 2018-09-09
 */
#ifndef __cx_io_ip_h__
#define __cx_io_ip_h__

#include <cx/base/defines.hpp>

namespace cx::io::ip {

	/**
	 * @brief
	 *
	 * @tparam ServiceType
	 */
	template < typename ServiceType > struct is_dgram_available : std::false_type {};

	/**
	 * @brief
	 *
	 * @tparam ServiceType
	 */
	template < typename ServiceType > struct is_stream_available : std::false_type {};


#pragma pack(push,1)

	inline namespace v4 {
		// from linux ip.h
		struct iphdr {
			uint8_t ip_v_hl;	/* version(4) header length(4) */
			uint8_t	ip_tos;		/* type of service */
			uint16_t ip_len;	/* total length */
			uint16_t ip_id;		/* identification */
			uint16_t ip_off;	/* fragment offset field */
			uint8_t	ip_ttl;		/* time to live */
			uint8_t	ip_p;		/* protocol */
			uint16_t ip_sum;	/* checksum */
			struct in_addr ip_src, ip_dst;	/* source and dest address */

			int ver(void) { return ip_v_hl >> 4; }
			int length(void) { return (ip_v_hl & 0x0f) * 4; }
		};
	}

	namespace v6 {
		// todo
	}
#pragma pack(pop)

}

#endif