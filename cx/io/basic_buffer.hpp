#ifndef __cx_io_basic_buffer_h__
#define __cx_io_basic_buffer_h__

#include <cx/cxdefine.hpp>

namespace cx::io {

	class buffer 
#if CX_PLATFORM == CX_P_WINDOWS
		: public WSABUF {
#else
		: public iovec{
#endif
	public:
		buffer(void) noexcept {
			this->base(nullptr);
			this->length(0);
		}

		buffer(void* ptr, std::size_t len) {
			this->base(ptr);
			this->length(len);
		}

		buffer(const std::string_view& msg) {
			this->base(const_cast<char*>(msg.data()));
			this->length(msg.size());
		}

		void reset(void* ptr , const std::size_t sz) {
			base(ptr);
			length(sz);
		}

#if CX_PLATFORM == CX_P_WINDOWS
		void* base(void) const { return buf; }

		std::size_t length(void) const { return len; }

		void* base(void* new_ptr) {
			void* old = buf;
			buf = static_cast< decltype(buf) >(new_ptr);
			return old;
		}

		std::size_t length(const std::size_t new_size) {
			std::size_t old = len;
			len = static_cast< decltype(len) >(new_size);
			return old;
		}

		WSABUF* raw_buffer(void) const {
			return static_cast<WSABUF*>(const_cast<buffer*>(this));
		}
#else
		void* base(void) const { return iov_base;; }

		std::size_t length(void) const { return iov_len; }

		void* base(void* new_ptr) {
			void* old = iov_base;
			iov_base = static_cast< decltype(iov_base) >(new_ptr);
			return old;
		}

		std::size_t length(const std::size_t new_size) {
			std::size_t old = iov_len;
			iov_len = static_cast< decltype(iov_len) >(new_size);
			return old;
		}

		struct iovec* raw_buffer(void) const {
			return static_cast<struct iovec*>(const_cast<buffer*>(this));
		}
#endif
	};
}

#endif
