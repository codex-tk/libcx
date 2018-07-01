/**
 */
#ifndef __cx_io_detail_reactor_socket_h__
#define __cx_io_detail_reactor_socket_h__

#include <cx/cxdefine.hpp>
#include <cx/io/basic_buffer.hpp>
#include <cx/io/handler_op.hpp>

#include <cx/io/ip/basic_address.hpp>
#include <cx/io/ip/option.hpp>

#include <cx/io/internal/reactor/basic_reactor.hpp>
#include <cx/io/internal/reactor/ops/connect_op.hpp>
#include <cx/io/internal/reactor/ops/accept_op.hpp>
#include <cx/io/internal/reactor/ops/read_op.hpp>
#include <cx/io/internal/reactor/ops/write_op.hpp>

namespace cx::io::internal::reactor::ip {

	template < typename ImplementationType, typename ServiceType >
	class basic_socket_service;

	template < typename ImplementationType
        , template <typename, int, int > class ServiceType
        , int Type, int Proto >
    class basic_socket_service< ImplementationType
        , ServiceType<ImplementationType, Type, Proto> > {
    public:
        using implementation_type = ImplementationType;
        using service_type = ServiceType<ImplementationType, Type, Proto>;
		using native_handle_type = typename implementation_type::native_handle_type;
        struct _handle : public implementation_type::basic_handle {
            service_type& service;
            _handle(service_type& svc) : service(svc) {}
            _handle(service_type& svc, native_handle_type raw_handle )
                : service(svc){
                this->fd = raw_handle;
            }
        };
        using handle_type = std::shared_ptr<_handle>;
        using address_type = cx::io::ip::basic_address< struct sockaddr_storage, Type, Proto >;

        static const native_handle_type invalid_native_handle = -1;

        handle_type make_shared_handle(service_type& svc) {
            return std::make_shared<_handle>(svc);
        }

        handle_type make_shared_handle(service_type& svc, native_handle_type raw_handle) {
            return std::make_shared<_handle>(svc,raw_handle);
        }

        basic_socket_service(implementation_type& impl)
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

        bool shutdown(const handle_type& handle , int how ) {
			if (handle.get() == nullptr || handle->fd == invalid_native_handle) {
				last_error(std::make_error_code(std::errc::invalid_argument));
				return false;
			}
			if (::shutdown(handle->fd , how ) == -1 ) {
				last_error(cx::system_error());
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

        void register_op( const handle_type& handle , int op_id , typename implementation_type::operation_type* op ){
            bool empty = handle->ops[op_id].empty();
			handle->ops[op_id].add_tail(op);
			if (handle.get() == nullptr || handle->fd == invalid_native_handle) {
				handle->drain_all_ops(this->implementation()
					, std::make_error_code(std::errc::invalid_argument));
				return;
			}
			if (empty) {
                int ops = (handle->ops[0].empty() ? 0 : cx::io::pollin ) | 
                    (handle->ops[1].empty() ? 0 : cx::io::pollout);
                if (!this->implementation().bind(handle, ops)) {
					handle->drain_all_ops(this->implementation(), this->last_error());
				}
			}
        }
    private:
        implementation_type& _implementation;
	};
}

#endif
