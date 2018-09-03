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
		using mux = MuxT;
		using operation = typename mux::operation;

		basic_engine(void)
			: _multiplexer(this) {
		}

		~basic_engine(void) {
		}

		mux& multiplexer(void) {
			std::lock_guard<cx::basic_lock> guard(_lock);
			return _multiplexer;
		}

		void post(cx::slist<operation>&& ops) {
			std::lock_guard<cx::basic_lock> guard(_lock);
			_ops.add_tail(std::move(ops));
			_multiplexer.wakeup();
		}

		void post(operation* op) {
			std::lock_guard<cx::basic_lock> guard(_lock);
			_ops.add_tail(op);
			_multiplexer.wakeup();
		}

		void run(const std::chrono::milliseconds& wait_ms) {
			_multiplexer.run(wait_ms);
			cx::slist<operation> ops;
			do {
				std::lock_guard<cx::basic_lock> guard(_lock);
				ops.add_tail(std::move(_ops));
			} while (0);
			while (!ops.empty()) {
				auto op = ops.remove_head();
				(*op)();
			}
		}

	private:
		cx::basic_lock _lock;
		cx::slist<operation> _ops;
		mux _multiplexer;
	};

}

#endif