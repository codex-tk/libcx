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

namespace cx::io::ip {

	template < typename EngineType > class basic_service {
	public:
		using engine_type = EngineType;
		using mux_type = typename engine_type::mux_type;
		using descriptor_type = typename mux_type::descriptor_type;
		using socket_type = typename mux_type::socket_type;

		template < int Type, int Proto >
		static bool open(const descriptor_type& descriptor,
			const cx::io::ip::basic_address<Type, Proto>& addr,
			std::error_code& ec)
		{
			if (!descriptor) {
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
			if (fd == mux_type::invalid_socket) {
				ec = cx::system_error();
				return false;
			}
			mux_type::socket_handle(descriptor, fd);
			return true;
		}

		static void close(const descriptor_type& descriptor) {
			if (!descriptor)
				return;

			if (mux_type::socket_handle(descriptor) != mux_type::invalid_socket) {
#if defined(CX_PLATFORM_WIN32)
				::closesocket(mux_type::socket_handle(descriptor, mux_type::invalid_socket));
#else
				::close(mux_type::socket_handle(descriptor, mux_type::invalid_socket));
#endif
			}
		};

		template < int Type, int Proto >
		static bool bind(const descriptor_type& descriptor,
			const cx::io::ip::basic_address<Type, Proto>& addr,
			std::error_code& ec)
		{
			if (!mux_type::good(descriptor)) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return false;
			}
			if (::bind(mux_type::socket_handle(descriptor), addr.sockaddr(), addr.length()) == -1) {
				ec = cx::system_error();
				return false;
			}
			return true;
		}

		static bool listen(const descriptor_type& descriptor, int backlog, std::error_code& ec) {
			if (!mux_type::good(descriptor)) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return false;
			}
			if (::listen(mux_type::socket_handle(descriptor), backlog) == -1) {
				ec = cx::system_error();
				return false;
			}
			return true;
		}

		static bool shutdown(const descriptor_type& descriptor, int how,
			std::error_code& ec) {
			if (!mux_type::good(descriptor)) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return false;
			}
			if (::shutdown(mux_type::socket_handle(descriptor), how) == -1) {
				ec = cx::system_error();
				return false;
			}
			return true;
		}

		static int send(const descriptor_type& descriptor, void* buf, int sz, int flags, std::error_code& ec) {
			if (!mux_type::good(descriptor)) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return -1;
			}
			int ret = ::send(mux_type::socket_handle(descriptor), static_cast<const char*>(buf), sz, flags);
			if (ret == -1) {
				ec = cx::system_error();
				return -1;
			}
			return ret;
		}

		static int recv(const descriptor_type& descriptor, void* buf, int sz, int flags, std::error_code& ec) {
			if (!mux_type::good(descriptor)) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return -1;
			}
			int ret = ::recv(mux_type::socket_handle(descriptor), static_cast<char*>(buf), sz, flags);
			if (ret == -1) {
				ec = cx::system_error();
				return -1;
			}
			return ret;
		}

		static int sendto(const descriptor_type& descriptor,
			void* buf,
			int sz,
			int flags,
			const struct sockaddr* addr,
			socklen_t addr_sz,
			std::error_code& ec)
		{
			if (!mux_type::good(descriptor)) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return -1;
			}
			int ret = ::sendto(mux_type::socket_handle(descriptor),
				static_cast<const char*>(buf), sz, flags, addr, addr_sz);
			if (ret == -1) {
				ec = cx::system_error();
				return -1;
			}
			return ret;
		}

		static int recvfrom(const descriptor_type& descriptor,
			void* buf,
			int sz,
			int flags,
			struct sockaddr* addr,
			socklen_t* addr_sz_ptr,
			std::error_code& ec)
		{
			if (!mux_type::good(descriptor)) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return -1;
			}
			int ret = ::recvfrom(mux_type::socket_handle(descriptor),
				static_cast<char*>(buf), sz, flags, addr, addr_sz_ptr);
			if (ret == -1) {
				ec = cx::system_error();
				return -1;
			}
			return ret;
		}

		static bool connect(const descriptor_type& descriptor,
			const struct sockaddr* addr,
			socklen_t addr_sz,
			std::error_code& ec)
		{
			if (!mux_type::good(descriptor)) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return false;
			}
			if (::connect(mux_type::socket_handle(descriptor), addr, addr_sz) == 0) {
				return true;
			}
			if (errno == EINPROGRESS) {
				return true;
			}
			ec = cx::system_error();
			return false;
		}

		static bool accept(const descriptor_type& descriptor,
			const descriptor_type& accepted,
			struct sockaddr* addr,
			socklen_t* addr_sz,
			std::error_code& ec)
		{
			if (!mux_type::good(descriptor) || addr == nullptr || addr_sz == 0) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return false;
			}

			if (mux_type::good(accepted)) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return false;
			}

			socket_type fd = ::accept(mux_type::socket_handle(descriptor), addr, addr_sz);
			if (fd == mux_type::invalid_socket) {
				ec = cx::system_error();
				return false;
			}
			mux_type::socket_handle(accepted, fd);
			return true;
		}

		static int poll(const descriptor_type& descriptor, 
			int ops, 
			const std::chrono::milliseconds& ms,
			std::error_code& ec) {
			if (!mux_type::good(descriptor)) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return false;
			}
#if defined(CX_PLATFORM_WIN32)
			WSAPOLLFD pollfd = { 0 };
			pollfd.fd = mux_type::socket_handle(descriptor);
			pollfd.events = ((ops & cx::io::pollin) ? POLLRDNORM : 0) | ((ops & cx::io::pollout) ? POLLWRNORM : 0);
			int result = WSAPoll(&pollfd, 1, static_cast<int>(ms.count()));
			if (result == SOCKET_ERROR) {
				ec = cx::system_error();
				return -1;
			}
			if (result == 0) {
				ec = std::make_error_code(std::errc::timed_out);
				return 0;
			}
			ops = ((pollfd.revents & POLLRDNORM) ? cx::io::pollin : 0) | ((pollfd.revents & POLLWRNORM) ? cx::io::pollout : 0);
			return ops;
#else
			struct pollfd pfd = { 0 };
			pfd.fd = mux_type::socket_handle(descriptor);
			pfd.events = ops;
			int result = ::poll(&pfd, 1, ms.count());
			if (result == -1) {
				ec = cx::system_error();
				return -1;
			}
			if (result == 0) {
				ec = std::make_error_code(std::errc::timed_out);
				return 0;
			}
			return pfd.revents;
#endif
		}

		static bool local_address(const descriptor_type& descriptor, 
			struct sockaddr* addr,
			socklen_t* addr_sz_ptr,
			std::error_code& ec) 
		{
			if (!mux_type::good(descriptor)) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return false;
			}
			if (::getsockname(mux_type::socket_handle(descriptor), addr, addr_sz_ptr) == -1) {
				ec = cx::system_error();
				return false;
			}
			return true;
		}

		static bool remote_address(const descriptor_type& descriptor,
			struct sockaddr* addr,
			socklen_t* addr_sz_ptr,
			std::error_code& ec)
		{
			if (!mux_type::good(descriptor)) {
				ec = std::make_error_code(std::errc::invalid_argument);
				return false;
			}
			if (::getpeername(mux_type::socket_handle(descriptor), addr, addr_sz_ptr) == -1) {
				ec = cx::system_error();
				return false;
			}
			return true;
		}

	};
}


#endif