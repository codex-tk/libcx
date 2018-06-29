/**
 * @brief 
 * 
 * @file socket_service.hpp
 * @author ghtak
 * @date 2018-06-29
 */
#ifndef __cx_io_internal_reactor_ip_socket_service_h__
#define __cx_io_internal_reactor_ip_socket_service_h__

#include <cx/io/internal/reactor/basic_socket_service.hpp>

namespace cx::io::internal::reactor::ip {

	template < typename ImplementationType, int Type, int Proto >
	class socket_service
		: public basic_socket_service < ImplementationType,
		socket_service< ImplementationType, Type, Proto >>{
	public:
		using implementation_type = ImplementationType;
		using this_type = socket_service;
		using operation_type = typename implementation_type::operation_type;

		using base_type = basic_socket_service<implementation_type,this_type>;
		using address_type = typename base_type::address_type;
		using handle_type = typename base_type::handle_type;
		using native_handle_type = typename base_type::native_handle_type;

		struct _buffer {
			address_type address;
			cx::io::buffer buffer;

			_buffer(void* ptr, std::size_t len)
				: buffer(ptr, len) {}
			_buffer(void)
				: buffer(nullptr, 0) {}
		};
		using buffer_type = _buffer;
        
		using base_type::invalid_native_handle;
		using base_type::connect;
		using base_type::make_shared_handle;

		template < typename HandlerType > using read_op
			= cx::io::handler_op< cx::io::internal::reactor::ip::read_op<this_type>, HandlerType>;
		template < typename HandlerType > using write_op
			= cx::io::handler_op< cx::io::internal::reactor::ip::write_op<this_type>, HandlerType>;

		handle_type make_shared_handle(void) {
			return this->make_shared_handle(*this);
		}

		handle_type make_shared_handle(native_handle_type raw_handle) {
			return this->make_shared_handle(*this,raw_handle);
		}

		socket_service(implementation_type& impl)
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

		template < typename HandlerType >
		void async_write(const handle_type& handle, const buffer_type& buf, HandlerType&& handler) {
			write_op<HandlerType>* op
				= new write_op<HandlerType>(buf, std::forward<HandlerType>(handler));

			async_write(handle, op);
		}

		void async_write(const handle_type& handle, cx::io::basic_write_op<this_type>* op) {
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