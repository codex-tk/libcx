/**
 * @brief
 *
 * @file address.hpp
 * @author ghtak
 * @date 2018-05-26
 */
#ifndef __cx_io_ip_address_h__
#define __cx_io_ip_address_h__

#include <cx/io/io.hpp>
#include <vector>

namespace cx::io::ip {

	template < typename SockAddrT, int Type, int Proto >
	class basic_address {
	public:
		basic_address(void) noexcept : _length(sizeof(SockAddrT)) {
			memset(&_address, 0x00, sizeof(_address));
		}

		basic_address(struct sockaddr* addr_ptr, const socklen_t length)
			: _length(length) {
			memcpy(sockaddr(), addr_ptr, _length);
		}

		basic_address(const basic_address& rhs) : _length(rhs.length()) {
			memcpy(sockaddr(), rhs.sockaddr(), _length);
		}

		basic_address(const char* dst, const uint16_t port, const short family) {
			sockaddr()->sa_family = family;
			_length = family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
			switch (sockaddr()->sa_family) {
			case AF_INET:
				reinterpret_cast<struct sockaddr_in*>(sockaddr())->sin_port = htons(port);
				::inet_pton(family, dst,
					&(reinterpret_cast<struct sockaddr_in*>(sockaddr())->sin_addr));
				break;
			case AF_INET6:
				reinterpret_cast<struct sockaddr_in6*>(sockaddr())->sin6_port = htons(port);
				::inet_pton(family, dst,
					&(reinterpret_cast<struct sockaddr_in6*>(sockaddr())->sin6_addr));
				break;
                /*
#if !defined(CX_PLATFORM_WIN32)
			//case AF_UNIX:
			case PF_FILE:
				_address.sun_family = family;
				strcpy(_address.sun_path, dst);
				_length = sizeof(struct sockaddr_un);
				break;
#endif
*/
			default:
				assert(0);
				_length = 0;
				break;
			}
		}

		~basic_address(void) {
		}

		struct sockaddr* sockaddr(void) {
			return const_cast<struct sockaddr*>(
				static_cast<const basic_address *>(this)->sockaddr());
		}

		const struct sockaddr* sockaddr(void) const {
			return reinterpret_cast<const struct sockaddr*>(&_address);
		}

		socklen_t length(void) const {
			return _length;
		}

		socklen_t* length_ptr(void) {
			return &_length;
		}

		bool inet_ntop(char* out, const int len) const {
			struct sockaddr* psockaddr = const_cast<struct sockaddr*>(this->sockaddr());
			switch (psockaddr->sa_family) {
			case AF_INET:
				return ::inet_ntop(psockaddr->sa_family
					, &(reinterpret_cast<struct sockaddr_in*>(psockaddr)->sin_addr)
					, out
					, len) != nullptr;
			case AF_INET6:
				return ::inet_ntop(psockaddr->sa_family
					, &(reinterpret_cast<struct sockaddr_in6*>(psockaddr)->sin6_addr)
					, out
					, len) != nullptr;
                /*
#if !defined(CX_PLATFORM_WIN32)
			case AF_UNIX:
			case PF_FILE:
				strncpy(out, _address.sun_path, len);
				return true;
#endif
*/
			default:
				assert(0);
				break;
			}
			return false;
		}

		const uint16_t family(void) const {
			return sockaddr()->sa_family;
		}

		int port(void) const {
			switch (family()) {
			case AF_INET: return ntohs(reinterpret_cast<const struct sockaddr_in*>(sockaddr())->sin_port);
			case AF_INET6: return ntohs(reinterpret_cast<const struct sockaddr_in6*>(sockaddr())->sin6_port);
			}
			return 0;
		}

		std::string to_string(void) const {
			char buf[MAX_PATH] = { 0 , };
			int len = 0;
			switch (family()) {
			case AF_INET:
				len += snprintf(buf + len, MAX_PATH - len, "AF_INET ");
				break;
			case AF_INET6:
				len += snprintf(buf + len, MAX_PATH - len, "AF_INET6 ");
				break;
                /*
#if !defined(CX_PLATFORM_WIN32)
			case AF_UNIX:
			case PF_FILE:
				len += snprintf(buf + len, MAX_PATH - len, "AF_UNIX ");
				break;
#endif
*/
			default:
				len += snprintf(buf + len, MAX_PATH - len, "UNKNOWN ");
				break;
			}
			if (this->inet_ntop(buf + len, MAX_PATH - len)) {
				len = strlen(buf);
				snprintf(buf + len, MAX_PATH - len, " (%d)", port());
				return std::string(buf);
			}
			return std::string("");
		}

		int type(void) const {
			return Type;
		}

		int proto(void) const {
			return Proto;
		}
	public:
		static basic_address any(const uint16_t port, const short family = AF_INET) {
			SockAddrT addr;
			memset(&addr, 0x00, sizeof(addr));
			socklen_t length = 0;
			switch (family) {
			case AF_INET:
				reinterpret_cast<struct sockaddr_in*>(&addr)->sin_family = family;
				reinterpret_cast<struct sockaddr_in*>(&addr)->sin_port = htons(port);
				reinterpret_cast<struct sockaddr_in*>(&addr)->sin_addr.s_addr = htonl(INADDR_ANY);
				length = sizeof(struct sockaddr_in);
				break;
			case AF_INET6:
				reinterpret_cast<struct sockaddr_in6*>(&addr)->sin6_family = family;
				reinterpret_cast<struct sockaddr_in6*>(&addr)->sin6_flowinfo = 0;
				reinterpret_cast<struct sockaddr_in6*>(&addr)->sin6_port = htons(port);
				reinterpret_cast<struct sockaddr_in6*>(&addr)->sin6_addr = in6addr_any;
				length = sizeof(struct sockaddr_in6);
				break;
			default:
				break;
			}
			return basic_address(reinterpret_cast<struct sockaddr*>(&addr), length);
		}

		static std::vector< basic_address > resolve(const char* name
			, const uint16_t port
			, const uint16_t family = AF_UNSPEC) {
			struct addrinfo hints;
			memset(&hints, 0x00, sizeof(hints));
			hints.ai_flags = AI_PASSIVE;
			hints.ai_family = family;
			hints.ai_socktype = SOCK_STREAM;

			constexpr std::size_t max_string = 16;

			char port_str[max_string] = { 0 , };
			snprintf(port_str, max_string, "%d", port);

			std::vector< basic_address > addrs;

			struct addrinfo* result = nullptr;
			struct addrinfo* rp = nullptr;

			if (getaddrinfo(name, port_str, &hints, &result) == 0) {
				for (rp = result; rp != nullptr; rp = rp->ai_next) {
					basic_address addr(rp->ai_addr, static_cast<socklen_t>(rp->ai_addrlen));
					addrs.push_back(addr);
				}
				freeaddrinfo(result);
			}
			return addrs;
		}
		static std::vector< basic_address > anys(const uint16_t port
			, const short family = AF_UNSPEC) {
			return resolve(nullptr, port, family);
		}
	private:
		SockAddrT _address;
		socklen_t _length;
	};
}

#endif
