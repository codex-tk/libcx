
/**
 */
#ifndef __cx_io_ip_basic_socker_service_h__
#define __cx_io_ip_basic_socker_service_h__

#if CX_PLATFORM == CX_P_WINDOWS

#include <cx/cxdefine.hpp>
#include <cx/io/internal/win32/iocp/implementation.hpp>

namespace cx::io::ip {
inline namespace iocp{

template <typename ServiceType>
class basic_socker_service;

template <template <int, int> class ServiceType, int Type, int Proto>
class basic_socker_service<ServiceType<Type, Proto>>
{
  protected:
	~basic_socker_service(void) {}

  public:
	using implementation_type = cx::io::iocp::implementation;
	using address_type = cx::io::ip::basic_address<struct sockaddr_storage, Type, Proto>;
	using operation_type = typename implementation_type::operation_type;
	using service_type = ServiceType<Type, Proto>;

	using native_handle_type = SOCKET;
	static const native_handle_type invalid_native_handle = INVALID_SOCKET;

	struct _handle : public cx::io::iocp::implementation::basic_handle
	{
		_handle(service_type &svc)
			: service(svc)
		{
			this->fd.s = invalid_native_handle;
		}
		service_type &service;
	};

	using handle_type = std::shared_ptr<_handle>;

	basic_socker_service(implementation_type &impl)
		: _implementation(impl) {}

	bool open(const handle_type &handle, const address_type &address)
	{
		close(handle);
		handle->fd.s = ::WSASocketW(address.family(), address.type(), address.proto(), nullptr, 0, WSA_FLAG_OVERLAPPED);
		if (handle->fd.s == invalid_native_handle)
		{
			last_error(cx::system_error());
			return false;
		}
		implementation().bind(handle, 0);
		return true;
	}

	void close(const handle_type &handle)
	{
		if (handle && handle->fd.s != invalid_native_handle)
		{
			implementation().unbind(handle);
			::closesocket(handle->fd.s);
			handle->fd.s = invalid_native_handle;
		}
	}

	bool connect(const handle_type &handle, const address_type &address)
	{
		if (handle.get() == nullptr)
		{
			last_error(std::make_error_code(std::errc::invalid_argument));
			return false;
		}
		if (handle->fd.s == invalid_native_handle)
		{
			if (!open(handle, address))
			{
				return false;
			}
		}
		if (::connect(handle->fd.s, address.sockaddr(), address.length()) != SOCKET_ERROR)
		{
			return true;
		}
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return true;
		}
		last_error(cx::system_error());
		return false;
	}

	bool bind(const handle_type &handle, const address_type &address)
	{
		if (::bind(handle->fd.s, address.sockaddr(), address.length()) == SOCKET_ERROR)
		{
			last_error(cx::system_error());
			return false;
		}
		return true;
	}

	int poll(const handle_type &handle, int ops, const std::chrono::milliseconds &ms)
	{
		WSAPOLLFD pollfd = {0};
		pollfd.fd = handle->fd.s;
		pollfd.events = ((ops & cx::io::pollin) ? POLLRDNORM : 0) | ((ops & cx::io::pollout) ? POLLWRNORM : 0);
		int result = WSAPoll(&pollfd, 1, static_cast<int>(ms.count()));
		if (result == SOCKET_ERROR)
		{
			last_error(cx::system_error());
			return -1;
		}
		if (result == 0)
		{
			last_error(std::make_error_code(std::errc::timed_out));
			return 0;
		}
		ops = ((pollfd.revents & POLLRDNORM) ? cx::io::pollin : 0) | ((pollfd.revents & POLLWRNORM) ? cx::io::pollout : 0);
		return ops;
	}

	address_type local_address(const handle_type &handle)
	{
		this->last_error(std::error_code{});
		address_type addr;
		if (::getsockname(handle->fd.s, addr.sockaddr(), addr.length_ptr()) == SOCKET_ERROR)
		{
			last_error(cx::system_error());
		}
		return addr;
	}

	address_type remote_address(const handle_type &handle)
	{
		this->last_error(std::error_code{});
		address_type addr;
		if (::getpeername(handle->fd.s, addr.sockaddr(), addr.length_ptr()) == SOCKET_ERROR)
		{
			last_error(cx::system_error());
		}
		return addr;
	}

	template <typename T>
	bool set_option(const handle_type &handle, T &&opt)
	{
		if (!opt.set(handle->fd.s))
		{
			last_error(cx::system_error());
			return false;
		}
		return true;
	}

	template <typename T>
	bool get_option(const handle_type &handle, T &&opt)
	{
		if (!opt.get(handle->fd.s))
		{
			last_error(cx::system_error());
			return false;
		}
		return true;
	}

	bool good(const handle_type &handle)
	{
		if (handle)
		{
			return handle->fd.s != invalid_native_handle;
		}
		return false;
	}

	implementation_type &implementation(void)
	{
		return _implementation;
	}

	std::error_code last_error(std::error_code &&ec)
	{
		return _implementation.last_error(ec);
	}

	std::error_code last_error(void)
	{
		return _implementation.last_error();
	}

  private:
	implementation_type &_implementation;
};
} // namespace iocp
} // namespace cx::io::ip

#endif // #if CX_PLATFORM == CX_P_WINDOWS

#endif