#ifndef __cx_io_ip_header_h__
#define __cx_io_ip_header_h__

#include <cx/cxdefine.hpp>
#include <cx/core/util.hpp>

namespace cx::io::ip {

#pragma pack(push,1)

	inline namespace v4 {
		// from linux ip.h
		struct iphdr {
#if defined(CX_LITTLE_ENDIAN)    
			uint8_t	ip_hl : 4;		/* header length */
			uint8_t ip_v : 4;			/* version */
#endif
#if defined(CX_BIG_ENDIAN)    
			uint8_t	ip_v : 4;			/* version */
			uint8_t	ip_hl : 4;		/* header length */
#endif
			uint8_t	ip_tos;			/* type of service */
			uint16_t ip_len;			/* total length */
			uint16_t ip_id;			/* identification */
			uint16_t ip_off;			/* fragment offset field */
			uint8_t	ip_ttl;			/* time to live */
			uint8_t	ip_p;			/* protocol */
			uint16_t ip_sum;			/* checksum */
			struct	in_addr ip_src, ip_dst;	/* source and dest address */
		};

		struct icmphdr {
			uint8_t   icmp_type;
			uint8_t   icmp_code;
			uint16_t  icmp_checksum;
			uint16_t  icmp_id;
			uint16_t  icmp_sequence;
		};
	}

	namespace v6 {
		// todo
	}

#pragma pack(pop)

}

#endif