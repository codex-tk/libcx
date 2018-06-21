/**
 *
 * */
#ifndef __cx_io_ip_basic_accept_context_h__
#define __cx_io_ip_basic_accept_context_h__

namespace cx::io::ip {
    
    template < typename ServiceType >
    class basic_accept_context {
    public:
        using native_handle_type = typename ServiceType::native_handle_type;
        using handle_type = typename ServiceType::handle_type;

		basic_accept_context(ServiceType& svc)
			: _service(svc), _handle(ServiceType::invalid_native_handle) {}

		basic_accept_context(ServiceType& svc, native_handle_type handle)
			: _service(svc), _handle(handle) {}

		basic_accept_context(const basic_accept_context& rhs)
			: _service(rhs._service), _handle(rhs._handle) {}

        handle_type make_shared_handle( ServiceType& service ) {
            return service.make_shared_handle(_handle);
        }

        handle_type make_shared_handle( void ) {
            return _service.make_shared_handle(_handle);
        }

		ServiceType& service(void) {
			return _service;
		}

		native_handle_type handle(native_handle_type h) {
			std::swap(_handle, h);
			return h;
		}

		native_handle_type handle(void) {
			return _handle;
		}

		explicit operator bool(void) const {
			return _handle != ServiceType::invalid_native_handle;
		}
    private:
        ServiceType& _service;
        native_handle_type _handle;
    };
}

#endif	