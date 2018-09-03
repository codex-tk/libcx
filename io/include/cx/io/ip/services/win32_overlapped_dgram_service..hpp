/**
 * @brief
 *
 * @file win32_overlapped_dgram_service..hpp
 * @author ghtak
 * @date 2018-09-01
 */

#ifndef __cx_io_ip_win32_overlapped_dgram_service_h__
#define __cx_io_ip_win32_overlapped_dgram_service_h__

#include <cx/base/defines.hpp>

#if defined(CX_PLATFORM_WIN32)

#include <cx/base/error.hpp>

#include <cx/io/descriptor.hpp>
#include <cx/io/ip/basic_address.hpp>

namespace cx::io::ip {

	template < typename MuxT > class win32_overlapped_dgram_service {
	public:
		using mux_t = MuxT;

		template < int Type, int Proto >
		static bool open(mux_t& mux,
			cx::io::descriptor_ptr& fd,
			const cx::io::ip::basic_address<Type, Proto>& addr)
		{
			std::error_code ec;
			return open(mux, fd, addr, ec);
		}

		template < int Type, int Proto >
		static bool open(mux_t& mux,
			cx::io::descriptor_ptr& fd,
			const cx::io::ip::basic_address<Type, Proto>& addr,
			std::error_code& ec)
		{
			close(mux, fd);
			SOCKET fd = ::WSASocketW(af, type, proto, nullptr, 0, WSA_FLAG_OVERLAPPED);
			if (fd == INVALID_SOCKET) {
				ec = cx::system_error();
				return false;
			}
			fd->native_handle(fd);
			return true;
		}

		static void close(mux_t& mux,
			cx::io::descriptor_ptr& fd)
		{
			if (fd->native_handle<SOCKET>() != INVALID_SOCKET) {
				::closesocket(fd->native_handle<SOCKET>());
			fd->native_handle(INVALID_SOCKET);
		}
	};
}

#endif

#endif