/**
 * @brief
 *
 * @file descriptor.hpp
 * @author ghtak
 * @date 2018-08-26
 */
#ifndef __cx_io_descriptor_h__
#define __cx_io_descriptor_h__

#include <cx/io/io.hpp>
#include <cx/base/defines.hpp>
#include <cx/base/slist.hpp>

namespace cx::io {

	class engine;
	class operation;
	/**
	 * @brief
	 *
	 */
	class descriptor :
		public std::enable_shared_from_this<descriptor> {
	public:
		descriptor(void);
		~descriptor(void) = default;

		void put(cx::io::type t, cx::io::operation* op);

		int handle_event(cx::io::engine& engine, int revt);
	public:
		template <typename T> T fd(void) {
			return reinterpret_cast<T>(_fd);
		}

		template <typename T> T fd(T fd) {
			T old = this->fd<T>();
			_fd = fd;
			return old;
		}
	private:
		std::ptrdiff_t _fd;
		cx::slist<cx::io::operation> _ops[2];
	};

	using descriptor_t = std::shared_ptr<descriptor>;
}

#endif