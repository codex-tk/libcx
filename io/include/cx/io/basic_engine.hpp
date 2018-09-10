/**
 * @brief
 *
 * @file basic_engine.hpp
 * @author ghtak
 * @date 2018-08-26
 */

#ifndef __cx_io_basic_engine_h__
#define __cx_io_basic_engine_h__

#include <cx/base/defines.hpp>
#include <cx/base/noncopyable.hpp>

#include <cx/base/basic_lock.hpp>

#if defined(CX_PLATFORM_WIN32)
#include <cx/io/mux/completion_port.hpp>
#elif defined(CX_PLATFORM_LINUX)
#include <cx/io/mux/epoll.hpp>
#endif

namespace cx::io {

	/**
	 * @brief
	 *
	 */
	template <typename MuxT> class basic_engine
		: private cx::noncopyable {
	public:
		using mux_type = MuxT;
		using operation_type = typename mux_type::operation_type;
		using descriptor_type = typename mux_type::descriptor_type;

		basic_engine(void)
			: _multiplexer(*this) {
		}

		~basic_engine(void) {
		}

		mux_type& multiplexer(void) {
			return _multiplexer;
		}

		void post(cx::slist<operation_type>&& ops) {
			std::lock_guard<cx::lock> guard(_lock);
			_ops.add_tail(std::move(ops));
			_multiplexer.wakeup();
		}

		void post(operation_type* op) {
			std::lock_guard < cx::lock > guard(_lock);
			_ops.add_tail(op);
			_multiplexer.wakeup();
		}

		int run(const std::chrono::milliseconds& wait_ms) {
			int handled = 0;
			handled += _multiplexer.run(wait_ms);
			cx::slist<operation_type> ops;
			do {
				std::lock_guard<cx::lock> guard(_lock);
				ops.add_tail(std::move(_ops));
			} while (0);
			while (!ops.empty()) {
				auto op = ops.remove_head();
				(*op)();
				++handled;
			}
			return handled;
		}

	private:
		cx::lock _lock;
		cx::slist<operation_type> _ops;
		mux_type _multiplexer;
	};

}

#endif