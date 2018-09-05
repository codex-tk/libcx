/**
 * @brief 
 * 
 * @file basic_service.hpp
 * @author ghtak
 * @date 2018-09-05
 */
#ifndef __cx_io_ip_basic_service_h__
#define __cx_io_ip_basic_service_h__

#include <cx/base/defines.hpp>
#include <cx/base/error.hpp>
#include <cx/io/ip/basic_address.hpp>

namespace cx::io::ip{

    template < typename EngineType > class basic_service{
    public:
        using engine_type = EngineType;
		using mux_type = typename engine_type::mux_type;
		using descriptor_type = typename mux_type::descriptor_type;

#if defined(CX_PLATFORM_WIN32)
		using socket_type = SOCKET;
		static const socket_type invalid_socket = INVALID_SOCKET;
#else
		using socket_type = int;
		static const socket_type invalid_socket = -1;
#endif
template < int Type, int Proto >
		static bool open(const descriptor_type& descriptor,
			const cx::io::ip::basic_address<Type, Proto>& addr,
			std::error_code& ec)
		{
			if (descriptor.get() == nullptr) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return false;
			}
			close(descriptor);
#if defined(CX_PLATFORM_WIN32)
			socket_type fd = ::WSASocketW(addr.family(),
				addr.type(),
				addr.proto(),
				nullptr, 0,
				WSA_FLAG_OVERLAPPED);

#else
			socket_type fd = ::socket(addr.family(),
				addr.type(),
				addr.proto());
#endif
			
			if (fd == invalid_socket) {
				ec = cx::system_error();
				return false;
			}
			descriptor->socket_handle(fd);
			return true;
		}

		static void close(const descriptor_type& descriptor) {
			if (!descriptor) {
				return;
			}
			if (descriptor->socket_handle() != invalid_socket) {
#if defined(CX_PLATFORM_WIN32)
				::closesocket(descriptor->socket_handle(invalid_socket));
#else
				::close(descriptor->socket_handle(invalid_socket));
#endif
			}
		};

		template < int Type, int Proto >
		static bool bind(const descriptor_type& descriptor,
			const cx::io::ip::basic_address<Type, Proto>& addr,
			std::error_code& ec)
		{
			if (descriptor.get() == nullptr || descriptor->socket_handle() == invalid_socket) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return false;
			}
			if (::bind(descriptor->socket_handle(), addr.sockaddr(), addr.length()) == -1) {
				ec = cx::system_error();
				return false;
			}
			return true;
		}

		static bool shutdown(const descriptor_type& descriptor, int how,
			std::error_code& ec) {
			if (descriptor.get() == nullptr || descriptor->socket_handle() == invalid_socket) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return false;
			}
			if (::shutdown(descriptor->socket_handle(), how) == -1) {
				ec = cx::system_error();
				return false;
			}
			return true;
		}
    };
}


#endif