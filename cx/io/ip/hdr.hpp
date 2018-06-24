#ifndef __cx_io_ip_header_h__
#define __cx_io_ip_header_h__

#include <cx/cxdefine.hpp>

namespace cx::io::ip {

#pragma pack(push,1)

	inline namespace v4 {
		// from linux ip.h
		struct ip {
#if defined(CX_LITTLE_ENDIAN)    
			uint8_t	ip_hl : 4,		/* header length */
				ip_v : 4;			/* version */
#endif
#if defined(CX_BIG_ENDIAN)    
			uint8_t	ip_v : 4,			/* version */
				ip_hl : 4;		/* header length */
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

		struct icmp {
			uint8_t	icmp_type;		/* type of message, see below */
			uint8_t	icmp_code;		/* type sub code */
			uint16_t icmp_cksum;		/* ones complement cksum of struct */
			union {
				uint8_t ih_pptr;			/* ICMP_PARAMPROB */
				struct in_addr ih_gwaddr;	/* ICMP_REDIRECT */
				struct ih_idseq {
					uint16_t icd_id;
					uint16_t icd_seq;
				} ih_idseq;
				int ih_void;
			} icmp_hun;
#define	icmp_pptr	icmp_hun.ih_pptr
#define	icmp_gwaddr	icmp_hun.ih_gwaddr
#define	icmp_id		icmp_hun.ih_idseq.icd_id
#define	icmp_seq	icmp_hun.ih_idseq.icd_seq
#define	icmp_void	icmp_hun.ih_void
			union {
				struct id_ts {
					uint32_t its_otime;
					uint32_t its_rtime;
					uint32_t its_ttime;
				} id_ts;
				struct id_ip {
					struct ip idi_ip;
					/* options and then 64 bits of data */
				} id_ip;
				u_long	id_mask;
				char	id_data[1];
			} icmp_dun;
#define	icmp_otime	icmp_dun.id_ts.its_otime
#define	icmp_rtime	icmp_dun.id_ts.its_rtime
#define	icmp_ttime	icmp_dun.id_ts.its_ttime
#define	icmp_ip		icmp_dun.id_ip.idi_ip
#define	icmp_mask	icmp_dun.id_mask
#define	icmp_data	icmp_dun.id_data
		};
	}

	namespace v6 {
		// todo
	}

#pragma pack(pop)

}

#endif