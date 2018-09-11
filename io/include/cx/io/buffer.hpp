/**
 * @brief
 *
 * @file buffer.hpp
 * @author ghtak
 * @date 2018-09-09
 */
#ifndef __cx_io_buffer_h__
#define __cx_io_buffer_h__

#include <cx/base/defines.hpp>
#include <cx/base/basic_buffer.hpp>

namespace cx::io {

	class buffer
#if defined(CX_PLATFORM_WIN32)
		: public WSABUF{
		using size_type = decltype(len);
#else
		: public iovec{
		using size_type = decltype(iov_len);
#endif
	public:
		buffer(void) noexcept {
			this->base(nullptr);
			this->length(0);
		}

		buffer(void* ptr, size_type len) {
			this->base(ptr);
			this->length(len);
		}

		buffer(const std::string_view& msg) {
			this->base(const_cast<char*>(msg.data()));
			this->length(static_cast<size_type>(msg.size()));
		}

		void reset(void* ptr , const size_type sz) {
			base(ptr);
			length(sz);
		}

#if defined(CX_PLATFORM_WIN32)
		void* base(void) const { return buf; }

		size_type length(void) const { return len; }

		void* base(void* new_ptr) {
			void* old = buf;
			buf = static_cast<decltype(buf)>(new_ptr);
			return old;
		}

		size_type length(const size_type new_size) {
			size_type old = len;
			len = static_cast<size_type>(new_size);
			return old;
		}

		WSABUF* raw_buffer(void) const {
			return static_cast<WSABUF*>(const_cast<buffer*>(this));
		}
#else
		void* base(void) const { return iov_base;; }

		size_type length(void) const { return iov_len; }

		void* base(void* new_ptr) {
			void* old = iov_base;
			iov_base = static_cast<decltype(iov_base)>(new_ptr);
			return old;
		}

		size_type length(const size_type new_size) {
			size_type old = iov_len;
			iov_len = static_cast<size_type>(new_size);
			return old;
		}

		struct iovec* raw_buffer(void) const {
			return static_cast<struct iovec*>(const_cast<buffer*>(this));
		}
#endif
	};

}

#endif
