/**
 * @brief
 *
 * @file basic_address.hpp
 * @author ghtak
 * @date 2018-08-27
 */

#ifndef __cx_io_ip_basic_address_h__
#define __cx_io_ip_basic_address_h__

#include <cx/base/defines.hpp>

namespace cx::io::ip {

	template <int Type, int Proto> class basic_address {
	public:
		basic_address(void) noexcept {
			memset(&_address, 0x00, sizeof(_address));
		}

		basic_address(struct sockaddr* addr_ptr, const socklen_t length) {
			memcpy(&_address, addr_ptr, length);
		}

		basic_address(const basic_address& rhs) {
			memcpy(&_address, rhs.sockaddr(), sizeof(_address));
		}

		basic_address(const char* src, const uint16_t port, const short family) {
			_address.family = family;
			void* dst = nullptr;
			switch (_address.family) {
			case AF_INET:
				_address.v4.sin_port = htons(port);
				dst = &(_address.v4.sin_addr);
				break;
			case AF_INET6:
				_address.v6.sin6_port = htons(port);
				dst = &(_address.v6.sin6_addr);
				break;
			default:
				assert(0);
				break;
			}
			::inet_pton(family, src, dst);
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
			switch (_address.family) {
			case AF_INET:
				return sizeof(struct sockaddr_in);
			case AF_INET6:
				return sizeof(struct sockaddr_in6);
			}
			return 0;
		}

		bool inet_ntop(char* out, const int len) const {
			switch (_address.family) {
			case AF_INET:
			case AF_INET6:
				return ::inet_ntop(_address.family,
					_address.family == AF_INET ?
					reinterpret_cast<const void*>(&(_address.v4.sin_addr)) :
					reinterpret_cast<const void*>(&(_address.v6.sin6_addr)),
					out, len);
			default:
				assert(0);
				break;
			}
			return false;
		}

		const uint16_t family(void) const {
			return _address.family;
		}

		int port(void) const {
			switch (_address.family) {
			case AF_INET:
				return ntohs(_address.v4.sin_port);
			case AF_INET6:
				return ntohs(_address.v6.sin6_port);
			default:
				assert(0);
				break;
			}
			return 0;
		}

		std::string to_string(void) const {
			char buf[MAX_PATH] = { 0 , };
			struct _family_name {
				const char* operator()(const uint16_t family) {
					switch (family) {
					case AF_INET:  return "AF_INET";
					case AF_INET6: return "AF_INET6";
					}
					return "UNKNOWN";
				}
			};
			_family_name family_name;
			int len = snprintf(buf, MAX_PATH, "%s ", family_name(_address.family));
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
			basic_address address;
			address.family = family;
			switch (family) {
			case AF_INET:
				address._address.v4.sin_port = htons(port);
				address._address.v4.sin_addr.s_addr = htonl(INADDR_ANY);
				break;
			case AF_INET6:
				address._address.v6.sin6_flowinfo = 0;
				address._address.v6.sin6_port = htons(port);
				address._address.v6.sin6_addr = in6addr_any;
				break;
			default:
				assert(0);
				break;
			}
			return address;
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
		union {
			struct sockaddr_in v4;
			struct sockaddr_in6 v6;
			uint16_t family;
		} _address;
	};
}

#endif