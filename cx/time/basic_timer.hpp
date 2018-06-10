/**
*/
#ifndef _cx_time_basic_timer_h__
#define _cx_time_basic_timer_h__

namespace cx::time {

	template < typename ServiceType >
	class basic_timer {
	public:
		using handle_type = typename ServiceType::handle_type;

		template < typename EngineType >
		basic_timer(EngineType& e)
			: _service(e.service<ServiceType>())
			, _handle(_service.make_shared_handle())
		{
		}

		void expired_at(const std::chrono::system_clock::time_point& tp) {
			_handle->expired_at = tp;
		}

		void handler(const std::function< void(const std::error_code&)>& func) {
			_handle->handler = func;
		}

		void fire(void) {
			_service.fire(_handle);
		}

		bool cancel(void) {
			return _service.cancel(_handle);
		}
	private:
		ServiceType& _service;
		handle_type _handle;
	};


}
#endif