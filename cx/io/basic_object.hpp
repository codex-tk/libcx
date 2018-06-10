/**
 */
#ifndef __cx_io_basic_object_h__
#define __cx_io_basic_object_h__

namespace cx::io {

	template < typename ServiceType >
	class basic_object {
	public:
		using implementation_type = typename ServiceType::implementation_type;
		using handle_type = typename ServiceType::handle_type;
		using address_type = typename ServiceType::address_type;
		using buffer_type = typename ServiceType::buffer_type;

		template < typename EngineType >
		basic_object(EngineType& engine)
			: _service(engine.service<ServiceType>())
			, _handle(engine.service<ServiceType>()
				.make_shared_handle())
		{
		}

		basic_object(ServiceType& service, handle_type handle)
			: _service(service)
			, _handle(handle)
		{
		}

		~basic_object(void) {
			if (_handle.use_count() == 1)
				_service.close(_handle);
		}

		handle_type handle(void) const {
			return _handle;
		}

		ServiceType& service(void) const {
			return _service;
		}

		bool open(const address_type& address) {
			return _service.open(_handle, address);
		}

		int write(const buffer_type& buf) {
			return _service.write(_handle, buf);
		}

		int read(buffer_type& buf) {
			return _service.read(_handle, buf);
		}

		void close(void) {
			_service.close(_handle);
		}

		explicit operator bool() const {
			return _service.good(_handle);
		}
	public:
		template < typename HandlerType >
		void async_write(const buffer_type& buf, HandlerType&& handler) {
			service().async_write(handle(), buf, std::forward<HandlerType>(handler));
		}

		template < typename HandlerType >
		void async_read(const buffer_type& buf, HandlerType&& handler) {
			service().async_read(handle(), buf, std::forward<HandlerType>(handler));
		}
	private:
		ServiceType& _service;
		handle_type _handle;
	};

}

#endif