/**
 */
#ifndef __cx_io_detail_reactor_socket_h__
#define __cx_io_detail_reactor_socket_h__

#include <cx/cxdefine.hpp>
#include <cx/io/basic_buffer.hpp>
#include <cx/io/handler_op.hpp>

#include <cx/io/ip/basic_address.hpp>
#include <cx/io/ip/option.hpp>

#include <cx/io/internal/reactor/reactor.hpp>
#include <cx/io/internal/reactor/reactor_connect_op.hpp>
#include <cx/io/internal/reactor/reactor_accept_op.hpp>
#include <cx/io/internal/reactor/reactor_read_op.hpp>
#include <cx/io/internal/reactor/reactor_write_op.hpp>

namespace cx::io::ip {

	template < typename ImplementationType, typename ServiceType >
	class basic_reactor_socket_service;

	template < typename ImplementationType
		, template <typename, int, int > class ServiceType
		, int Type, int Proto >
		class basic_reactor_socket_service< ImplementationType
		, ServiceType<ImplementationType, Type, Proto> > {
		public:
			using implementation_type = ImplementationType;
			using service_type = ServiceType<ImplementationType, Type, Proto>;
			struct _handle : public implementation_type::basic_handle {
				service_type& service;

				_handle(service_type& svc)
					: service(svc) {}

			};
			using handle_type = std::shared_ptr<_handle>;
			using address_type = cx::io::ip::basic_address< struct sockaddr_storage, Type, Proto >;

			using native_handle_type = typename implementation_type::native_handle_type;
			static const native_handle_type invalid_native_handle = -1;

			handle_type make_shared_handle(service_type& svc) {
				return std::make_shared<_handle>(svc);
			}

			basic_reactor_socket_service(implementation_type& impl)
				: _implementation(impl) {}

			bool open(const handle_type& handle, const address_type& address) {
				close(handle);
				handle->fd = ::socket(address.family()
					, address.type()
					, address.proto());
				if (handle->fd == invalid_native_handle) {
					this->last_error(cx::system_error());
					return false;
				}
				return true;
			}

			void close(const handle_type& handle) {
				if (handle && handle->fd != invalid_native_handle) {
					implementation().unbind(handle);
					::close(handle->fd);
					handle->fd = -1;
					handle->drain_all_ops(implementation()
						, std::make_error_code(std::errc::operation_canceled));
				}
			}

			bool connect(const handle_type& handle, const address_type& address) {
				if (handle.get() == nullptr) {
					this->last_error(std::make_error_code(std::errc::invalid_argument));
					return false;
				}
				if (handle->fd == invalid_native_handle) {
					if (!open(handle, address)) {
						return false;
					}
				}
				if (::connect(handle->fd, address.sockaddr(), address.length()) == 0) {
					return true;
				}
				if (errno == EINPROGRESS) {
					return true;
				}
				this->last_error(cx::system_error());
				return false;
			}

			bool bind(const handle_type& handle, const address_type& address) {
				if (::bind(handle->fd, address.sockaddr(), address.length()) == -1) {
					this->last_error(cx::system_error());
					return false;
				}
				return true;
			}

			int poll(const handle_type& handle
				, int ops
				, const std::chrono::milliseconds& ms)
			{
				struct pollfd pfd = { 0 };
				pfd.fd = handle->fd;
				pfd.events = ops;
				int result = ::poll(&pfd, 1, ms.count());
				if (result == -1) {
					this->last_error(cx::system_error());
					return -1;
				}
				if (result == 0) {
					this->last_error(std::make_error_code(std::errc::timed_out));
					return 0;
				}
				return pfd.revents;
			}

			address_type local_address(const handle_type& handle) {
				this->last_error(std::error_code{});
				address_type addr;
				if (::getsockname(handle->fd, addr.sockaddr(), addr.length_ptr()) == -1) {
					this->last_error(cx::system_error());
				}
				return addr;
			}

			address_type remote_address(const handle_type& handle) {
				this->last_error(std::error_code{});
				address_type addr;
				if (::getpeername(handle->fd, addr.sockaddr(), addr.length_ptr()) == -1) {
					this->last_error(cx::system_error());
				}
				return addr;
			}

			template < typename T >
			bool set_option(const handle_type& handle, T&& opt) {
				if (!opt.set(handle->fd)) {
					this->last_error(cx::system_error());
					return false;
				}
				return true;
			}

			template < typename T >
			bool get_option(const handle_type& handle, T&& opt) {
				if (!opt.get(handle->fd)) {
					this->last_error(cx::system_error());
					return false;
				}
				return true;
			}

			bool good(const handle_type& handle) const {
				if (handle) {
					return handle->fd != invalid_native_handle;
				}
				return false;
			}

			implementation_type& implementation(void) {
				return _implementation;
			}

			std::error_code last_error(const std::error_code& ec) {
				return _implementation.last_error(ec);
			}

			std::error_code last_error(void) {
				return _implementation.last_error();
			}
		private:
			implementation_type& _implementation;
	};

	template < typename ImplementationType, int Type, int Proto >
	class reactor_socket_service
		: public basic_reactor_socket_service < ImplementationType,
		reactor_socket_service< ImplementationType, Type, Proto >>{
	public:
		using implementation_type = ImplementationType;
		using this_type = reactor_socket_service;
		using operation_type = typename reactor_base<ImplementationType>::operation_type;

		using base_type = basic_reactor_socket_service < ImplementationType, this_type >;
		using address_type = typename base_type::address_type;
		using handle_type = typename base_type::handle_type;
		using native_handle_type = typename base_type::native_handle_type;

		using base_type::invalid_native_handle;
		using base_type::connect;
		using base_type::make_shared_handle;

		struct _buffer {
			address_type address;
			cx::io::buffer buffer;

			_buffer(void* ptr, std::size_t len)
				: buffer(ptr, len) {}
			_buffer(void)
				: buffer(nullptr, 0) {}
		};
		using buffer_type = _buffer;

		template < typename HandlerType > using read_op
			= cx::io::handler_op< cx::io::reactor_read_op< this_type>, HandlerType>;
		template < typename HandlerType > using write_op
			= cx::io::handler_op< cx::io::reactor_write_op< this_type>, HandlerType>;

		handle_type make_shared_handle(void) {
			return this->make_shared_handle(*this);
		}

		handle_type make_shared_handle(native_handle_type handle) {
			auto h = make_shared_handle();
			h->fd = handle;
			return h;
		}

		reactor_socket_service(implementation_type& impl)
			: base_type(impl) {}

		int write(const handle_type& handle, const buffer_type& buf) {
			if (handle.get() == nullptr || handle->fd == invalid_native_handle) {
				this->last_error(std::make_error_code(std::errc::invalid_argument));
				return -1;
			}
			int ret = sendto(handle->fd
				, static_cast<const char*>(buf.buffer.base())
				, buf.buffer.length()
				, 0
				, buf.address.sockaddr()
				, buf.address.length());

			if (ret == -1) {
				this->last_error(cx::system_error());
				return -1;
			}
			return ret;
		}

		int read(const handle_type& handle, buffer_type& buf) {
			if (handle.get() == nullptr || handle->fd == invalid_native_handle) {
				this->last_error(std::make_error_code(std::errc::invalid_argument));
				return -1;
			}
			int ret = recvfrom(handle->fd
				, static_cast<char*>(buf.buffer.base())
				, buf.buffer.length()
				, 0
				, buf.address.sockaddr()
				, buf.address.length_ptr());

			if (ret == -1) {
				this->last_error(cx::system_error());
				return -1;
			}
			return ret;
		}

		int shutdown(const handle_type&, int) {
			return 0;
		}

		template < typename HandlerType >
		void async_write(const handle_type& handle, const buffer_type& buf, HandlerType&& handler) {
			write_op<HandlerType>* op
				= new write_op<HandlerType>(buf, std::forward<HandlerType>(handler));

			async_write(handle, op);
		}

		void async_write(const handle_type& handle, cx::io::reactor_write_op<this_type>* op) {
			if (handle->ops[1].add_tail(op) == 0) {
				int ops = cx::io::pollout | (handle->ops[0].head() ? cx::io::pollin : 0);
				if (!this->implementation().bind(handle, ops)) {
					handle->drain_all_ops(this->implementation(), this->last_error());
				}
			}
			else {
				if (handle.get() == nullptr || handle->fd == invalid_native_handle) {
					handle->drain_all_ops(this->implementation()
						, std::make_error_code(std::errc::invalid_argument));
				}
			}
		}

		template < typename HandlerType >
		void async_read(const handle_type& handle, const buffer_type& buf, HandlerType&& handler) {
			read_op<HandlerType>* op
				= new read_op<HandlerType>(buf, std::forward<HandlerType>(handler));

			async_read(handle, op);
		}

		void async_read(const handle_type& handle, cx::io::reactor_read_op<this_type>* op) {
			if (handle->ops[0].add_tail(op) == 0) {
				int ops = cx::io::pollin | (handle->ops[1].head() ? cx::io::pollout : 0);
				if (!this->implementation().bind(handle, ops)) {
					handle->drain_all_ops(this->implementation(), this->last_error());
				}
			}
			else {
				if (handle.get() == nullptr || handle->fd == invalid_native_handle) {
					handle->drain_all_ops(this->implementation()
						, std::make_error_code(std::errc::invalid_argument));
				}
			}
		}

		bool write_complete(const handle_type& handle, cx::io::basic_write_op<this_type>* op) {
			int ret = this->write(handle, op->buffer());
			if (ret <= 0) {
				op->error(cx::system_error());
			}
			op->io_size(ret);
			return true;
		}

		bool read_complete(const handle_type& handle, cx::io::basic_read_op<this_type>* op) {
			int ret = this->read(handle, op->buffer());
			if (ret < 0) {
				op->error(cx::system_error());
			}
			op->io_size(ret);
			return true;
		}

		using read_op_type = std::shared_ptr<cx::io::reactor_read_op<this_type>>;
		using write_op_type = std::shared_ptr<cx::io::reactor_write_op<this_type>>;

		template < typename HandlerType >
		read_op_type make_read_op(HandlerType&& handler) {
			return std::make_shared<cx::io::reusable_handler_op< typename read_op_type::element_type, HandlerType>>(
				std::forward<HandlerType>(handler)
				);
		}

		template < typename HandlerType >
		write_op_type make_write_op(HandlerType&& handler) {
			return std::make_shared<cx::io::reusable_handler_op< typename write_op_type::element_type, HandlerType>>(
				std::forward<HandlerType>(handler)
				);
		}

		void async_read(const handle_type& handle, read_op_type op) {
			async_read(handle, op.get());
		}

		void async_write(const handle_type& handle, write_op_type op) {
			async_write(handle, op.get());
		}
	};

	template <typename ImplementationType>
	class reactor_socket_service< ImplementationType, SOCK_STREAM, IPPROTO_TCP >
		: public basic_reactor_socket_service < ImplementationType,
		reactor_socket_service< ImplementationType, SOCK_STREAM, IPPROTO_TCP >> {
	public:
		using implementation_type = ImplementationType;
		using this_type = reactor_socket_service< ImplementationType, SOCK_STREAM, IPPROTO_TCP >;
		using buffer_type = cx::io::buffer;
		using operation_type = typename reactor_base<ImplementationType>::operation_type;

		using base_type = basic_reactor_socket_service < ImplementationType, this_type >;
		using address_type = typename base_type::address_type;
		using handle_type = typename base_type::handle_type;
		using native_handle_type = typename base_type::native_handle_type;

		using base_type::invalid_native_handle;
		using base_type::connect;
		using base_type::make_shared_handle;

		template < typename HandlerType > using connect_op
			= cx::io::ip::reactor_connect_op< this_type, HandlerType>;
		template < typename HandlerType > using accept_op
			= cx::io::ip::reactor_accept_op< this_type, HandlerType>;
		template < typename HandlerType > using read_op
			= cx::io::handler_op< cx::io::reactor_read_op<this_type> , HandlerType>;
		template < typename HandlerType > using write_op
			= cx::io::handler_op< cx::io::reactor_write_op<this_type>, HandlerType>;

		handle_type make_shared_handle(void) {
			return this->make_shared_handle(*this);
		}

		handle_type make_shared_handle(native_handle_type handle) {
			auto h = make_shared_handle();
			h->fd = handle;
			return h;
		}

		reactor_socket_service(implementation_type& impl)
			: base_type(impl) {}

		int write(const handle_type& handle, const buffer_type& buf) {
			if (handle.get() == nullptr || handle->fd == invalid_native_handle) {
				this->last_error(std::make_error_code(std::errc::invalid_argument));
				return -1;
			}
			int ret = send(handle->fd, static_cast<const char*>(buf.base()), buf.length(), 0);
			if (ret == -1) {
				this->last_error(cx::system_error());
				return -1;
			}
			return ret;
		}

		int read(const handle_type& handle, buffer_type& buf) {
			if (handle.get() == nullptr || handle->fd == invalid_native_handle) {
				this->last_error(std::make_error_code(std::errc::invalid_argument));
				return -1;
			}
			int ret = recv(handle->fd, static_cast<char*>(buf.base()), buf.length(), 0);
			if (ret == -1) {
				this->last_error(cx::system_error());
				return -1;
			}
			return ret;
		}

		bool listen(const handle_type& handle, int backlog) {
			if (handle.get() == nullptr || handle->fd == invalid_native_handle) {
				this->last_error(std::make_error_code(std::errc::invalid_argument));
				return false;
			}
			if (::listen(handle->fd, backlog) == -1) {
				this->last_error(cx::system_error());
				return false;
			}
			return true;
		}

		bool shutdown(const handle_type& handle, int how) {
			if (handle.get() == nullptr || handle->fd == invalid_native_handle) {
				this->last_error(std::make_error_code(std::errc::invalid_argument));
				return false;
			}
			if (::shutdown(handle->fd, how) == -1) {
				this->last_error(cx::system_error());
				return false;
			}
			return true;
		}

		basic_accept_context<this_type> accept(const handle_type& handle, address_type& addr) {
			native_handle_type fd = ::accept(handle->fd, addr.sockaddr(), addr.length_ptr());
			return basic_accept_context<this_type>(*this, fd);
		}

		template < typename HandlerType >
		void async_connect(const handle_type& handle, const address_type& addr, HandlerType&& handler) {
			connect_op< HandlerType >* op
				= new connect_op< HandlerType >(addr,
					std::forward<HandlerType>(handler));

			if (this->connect(handle, addr)) {
				if (this->implementation().bind(handle, cx::io::pollout)) {
					handle->ops[1].add_tail(op);
					return;
				}
			}
			op->error(this->last_error());
			this->implementation().post(op);
		}

		template < typename HandlerType >
		void async_write(const handle_type& handle, const buffer_type& buf, HandlerType&& handler) {
			write_op<HandlerType>* op
				= new write_op<HandlerType>(buf, std::forward<HandlerType>(handler));

			async_write(handle, op);
		}

		void async_write(const handle_type& handle, cx::io::reactor_write_op<this_type>* op ) {
			if (handle->ops[1].add_tail(op) == 0) {
				int ops = cx::io::pollout | (handle->ops[0].head() ? cx::io::pollin : 0);
				if (!this->implementation().bind(handle, ops)) {
					handle->drain_all_ops(this->implementation(), this->last_error());
				}
			}
			else {
				if (handle.get() == nullptr || handle->fd == invalid_native_handle) {
					handle->drain_all_ops(this->implementation()
						, std::make_error_code(std::errc::invalid_argument));
				}
			}
		}

		template < typename HandlerType >
		void async_read(const handle_type& handle, const buffer_type& buf, HandlerType&& handler) {
			read_op<HandlerType>* op
				= new read_op<HandlerType>(buf, std::forward<HandlerType>(handler));

			async_read(handle, op);
		}

		void async_read(const handle_type& handle, cx::io::reactor_read_op<this_type>* op) {
			if (handle->ops[0].add_tail(op) == 0) {
				int ops = cx::io::pollin | (handle->ops[1].head() ? cx::io::pollout : 0);
				if (!this->implementation().bind(handle, ops)) {
					handle->drain_all_ops(this->implementation(), this->last_error());
				}
			}
			else {
				if (handle.get() == nullptr || handle->fd == invalid_native_handle) {
					handle->drain_all_ops(this->implementation()
						, std::make_error_code(std::errc::invalid_argument));
				}
			}
		}

		template < typename HandlerType >
		void async_accept(const handle_type& handle, HandlerType&& handler) {
			accept_op<HandlerType>* op =
				new accept_op<HandlerType>(*this, std::forward<HandlerType>(handler));

			if (handle->ops[0].add_tail(op) == 0) {
				int ops = cx::io::pollin | (handle->ops[1].head() ? cx::io::pollout : 0);
				if (!this->implementation().bind(handle, ops)) {
					handle->drain_all_ops(this->implementation(), this->last_error());
				}
			}
			else {
				if (handle.get() == nullptr || handle->fd == invalid_native_handle) {
					handle->drain_all_ops(this->implementation()
						, std::make_error_code(std::errc::invalid_argument));
				}
			}
		}

		bool connect_complete(const handle_type& handle, cx::io::ip::basic_connect_op<this_type>* op) {
			return true;
		}

		bool accept_complete(const handle_type& handle, cx::io::ip::basic_accept_op<this_type>* op) {
			native_handle_type fd = ::accept(handle->fd, op->address().sockaddr(), op->address().length_ptr());
			op->accept_context().handle(fd);
			if (fd == invalid_native_handle) {
				op->error(cx::system_error());
			}
			return true;
		}

		bool write_complete(const handle_type& handle, cx::io::basic_write_op<this_type>* op) {
			int ret = this->write(handle, op->buffer());
			if (ret < 0) {
				op->error(cx::system_error());
			}
			op->io_size(ret);
			return true;
		}

		bool read_complete(const handle_type& handle, cx::io::basic_read_op<this_type>* op) {
			int ret = this->read(handle, op->buffer());
			if (ret < 0) {
				op->error(cx::system_error());
			}
			op->io_size(ret);
			return true;
		}

		using read_op_type = std::shared_ptr<cx::io::reactor_read_op<this_type>>;
		using write_op_type = std::shared_ptr<cx::io::reactor_write_op<this_type>>;

		template < typename HandlerType >
		read_op_type make_read_op(HandlerType&& handler) {
			return std::make_shared<cx::io::reusable_handler_op< typename read_op_type::element_type, HandlerType>>(
				std::forward<HandlerType>(handler)
			);
		}

		template < typename HandlerType >
		write_op_type make_write_op(HandlerType&& handler) {
			return std::make_shared<cx::io::reusable_handler_op< typename write_op_type::element_type, HandlerType>>(
				std::forward<HandlerType>(handler)
			);
		}

		void async_read(const handle_type& handle, read_op_type op) {
			async_read(handle, op.get());
		}

		void async_write(const handle_type& handle, write_op_type op) {
			async_write(handle, op.get());
		}
	};

}

#endif
