/**
 * @brief
 *
 * @file descriptor.hpp
 * @author ghtak
 * @date 2018-08-26
 */
#ifndef __cx_io_descriptor_h__
#define __cx_io_descriptor_h__

#include <cx/base/defines.hpp>
#include <cx/base/slist.hpp>
#include <cx/base/noncopyable.hpp>

#include <cx/io/io.hpp>

namespace cx::io {
	namespace {
		template < typename U > struct _cast {
			static U apply(std::ptrdiff_t src) {
				return reinterpret_cast<U>(src);
			}
		};
		template <> struct _cast<int> {
			static int apply(std::ptrdiff_t src) {
				return static_cast<int>(src);
			}
		};
#if defined(CX_PLATFORM_WIN32)
		template <> struct _cast<SOCKET> {
			static SOCKET apply(std::ptrdiff_t src) {
				return static_cast<SOCKET>(src);
			}
		};
#endif
	}

	class engine;
	class operation;
	/**
	 * @brief
	 *
	 */
	class descriptor :
		public cx::noncopyable ,
		public std::enable_shared_from_this<descriptor> {
	public:
		descriptor(void);
		~descriptor(void) = default;

		void put(cx::io::type type, cx::io::operation* op);

		cx::io::operation* get(cx::io::type type);

		int handle_event(cx::io::engine& engine, int revt);
	public:
		template <typename T> T native_handle(void) {
			return _cast<T>::apply(_native_handle);
		}

		template <typename T> T native_handle(T h) {
			T old = this->native_handle<T>();
			_native_handle = h;
			return old;
		}
	private:
		std::ptrdiff_t _native_handle;
		cx::slist<cx::io::operation> _ops[2];
	};

	using descriptor_t = std::shared_ptr<descriptor>;
}

#endif