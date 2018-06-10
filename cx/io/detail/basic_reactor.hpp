/**
*/
#ifndef __cx_io_detail_basic_reactor_h__
#define __cx_io_detail_basic_reactor_h__

#include <cx/cxdefine.hpp>
#include <cx/slist.hpp>

namespace cx::io::detail {

	template < template ImplementationType , typename ServiceType >
	class basic_reactor {
	public:
		using implementation_type = ImplementationType;
		using service_type = ServiceType;
		class operation {
		public:
			operation(void)
				: _next(nullptr) {
				reset();
			}

			virtual ~operation(void) = default;
			int io_size(void) { return _io_size; }
			int io_size(int sz) {
				std::swap(_io_size, sz);
				return sz;
			}

			std::error_code error(void) { return _ec; }
			std::error_code error(const std::error_code& ec) {
				std::error_code old(_ec);
				_ec = ec;
				return old;
			}

			virtual int operator()(void) = 0;
			virtual bool before(void) = 0;

			operation* next(void) { return _next; }
			operation* next(operation* op) {
				std::swap(_next, op);
				return op;
			}
		private:
			std::error_code _ec;
			int _io_size;
			operation* _next;
		};
		using operation_type = operation;
		struct handle {
			handle(service_type& svc)
				: service(svc)
			{
				fd = -1;
			}

			std::recursive_mutex _mutex;
			int fd;
			cx::slist<operation> ops[2];
			service_type& service;

			int handle_events(int ops) {
				int ops_filter[2] = { cx::io::pollin , cx::io::pollout };
				int proc = 0;
				for (int i = 0; i < 2; ++i) {
					if (ops_filter[i] & ops) {
						operation* op = ops[i].head();
						if (op && op->before()) {
							ops[i].remove_head();
							proc += (*op)();
						}
					}
				}
				return proc;
			}
		};

		using handle_type = std::shared_ptr<handle>;

		basic_reactor(implementation_type& impl)
			: _implementation(impl)
		{}

		implementation_type& implementation(void) {
			return _implementation;
		}
	private:
		implementation_type& _implementation;
	};

	template < template ImplementationType, typename ServiceType >
	class basic_reactor_socket_service;

	template < template ImplementationType
		, template < int, int > class ServiceType, int Type, int Proto >
	class basic_reactor_socket_service< ImplementationType, ServiceType<Type, Proto> >
		: basic_reactor< ImplementationType, ServiceType<Type, Proto> >
	{
	public:
		using address_type = cx::io::ip::basic_address< struct sockaddr_storage, Type, Proto >;

		bool open(handle_type handle, const address_type& address) {
			close(handle);
			handle->fd = ::socket(address.family()
				, address.type()
				, address.proto());
			if (handle->fd != -1)
				return true;
			return false;
		}

		void close(handle_type handle) {
			if (handle && handle->fd != -1) {
				implementation().unbind(handle);
				::close(handle->fd);
				handle->fd = -1;
			}
		}

		bool connect(handle_type handle, const address_type& address) {
			if (::connect(handle->fd, address.sockaddr(), address.length()) == 0)
				return true;
			if ( errno == EINPROGRESS )
			    return true;
			return false;
		}

		bool bind(handle_type handle, const address_type& address) {
			return ::bind(handle->fd, address.sockaddr(), address.length()) != -1;
		}

		int poll(handle_type handle
			, int ops
			, const std::chrono::milliseconds& ms)
		{
			struct pollfd pollfd = { 0 };
			pollfd.fd = handle->fd;
			pollfd.events = ops;
			if (poll(&pollfd, 1, ms.count()) == -1) {
				return -1;
			}
			return pollfd.revents;
		}

		address_type local_address(handle_type handle) const {
			address_type addr;
			::getsockname(handle->fd, addr.sockaddr(), addr.length_ptr());
			return addr;
		}

		address_type remote_address(handle_type handle) const {
			address_type addr;
			::getpeername(handle->fd, addr.sockaddr(), addr.length_ptr());
			return addr;
		}

		template < typename T >
		bool set_option(handle_type handle, T&& opt) {
			return opt.set(handle->fd);
		}

		template < typename T >
		bool get_option(handle_type handle, T&& opt) {
			return opt.get(handle->fd);
		}

		bool good(handle_type handle) {
			if (handle) {
				return handle->fd != -1;
			}
			return false;
		}
	};

}

#endif