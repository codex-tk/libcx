/**
 */
#ifndef __cx_io_internal_reactor_ip_socket_service_tcp_h__
#define __cx_io_internal_reactor_ip_socket_service_tcp_h__

#include <cx/io/internal/reactor/socket_service.hpp>

namespace cx::io::internal::reactor::ip {

	template <typename ImplementationType>
	class socket_service< ImplementationType, SOCK_STREAM, IPPROTO_TCP >
		: public basic_socket_service < ImplementationType,
		socket_service< ImplementationType, SOCK_STREAM, IPPROTO_TCP >> {
	public:
		using implementation_type = ImplementationType;
		using this_type = socket_service< ImplementationType, SOCK_STREAM, IPPROTO_TCP >;
		using operation_type = typename basic_reactor<ImplementationType>::operation_type;

		using base_type = basic_socket_service < ImplementationType, this_type >;
		using address_type = typename base_type::address_type;
		using handle_type = typename base_type::handle_type;
		using native_handle_type = typename base_type::native_handle_type;

		using buffer_type = cx::io::buffer;
		
		using base_type::invalid_native_handle;
		using base_type::connect;
		using base_type::make_shared_handle;

		template < typename HandlerType > using connect_op
			= cx::io::internal::reactor::ip::connect_op< this_type, HandlerType>;
		template < typename HandlerType > using accept_op
			= cx::io::internal::reactor::ip::accept_op< this_type, HandlerType>;
		template < typename HandlerType > using read_op
			= cx::io::handler_op< cx::io::internal::reactor::ip::read_op<this_type> , HandlerType>;
		template < typename HandlerType > using write_op
			= cx::io::handler_op< cx::io::internal::reactor::ip::write_op<this_type>, HandlerType>;

		handle_type make_shared_handle(void) {
			return this->make_shared_handle(*this);
		}

		handle_type make_shared_handle(native_handle_type handle) {
			auto h = make_shared_handle();
			h->fd = handle;
			return h;
		}

		socket_service(implementation_type& impl)
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

		cx::io::ip::basic_accept_context<this_type> accept(const handle_type& handle, address_type& addr) {
			native_handle_type fd = ::accept(handle->fd, addr.sockaddr(), addr.length_ptr());
			return cx::io::ip::basic_accept_context<this_type>(*this, fd);
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

		void async_write(const handle_type& handle, cx::io::basic_write_op<this_type>* op ) {
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

		void async_read(const handle_type& handle, cx::io::basic_read_op<this_type>* op) {
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

		using read_op_type = std::shared_ptr<cx::io::internal::reactor::ip::read_op<this_type>>;
		using write_op_type = std::shared_ptr<cx::io::internal::reactor::ip::write_op<this_type>>;

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
