/**
 * @brief
 *
 * @file basic_operation.hpp
 * @author ghtak
 * @date 2018-08-26
 */
#ifndef __cx_io_basic_operation_h__
#define __cx_io_basic_operation_h__

#include <cx/base/defines.hpp>
#include <cx/base/noncopyable.hpp>
#include <cx/base/slist.hpp>

namespace cx::io {

	/**
	 * @brief
	 *
	 */
	template < typename DescriptorT >
	class basic_operation :
		private cx::noncopyable,
		public cx::slist<basic_operation<DescriptorT>>::node {
	public:
		using descriptor_type = DescriptorT;

		basic_operation(void) {}
		virtual ~basic_operation(void) = default;

		virtual void request(const descriptor_type&) {}

		/**
		 * @brief check io complete
		 *
		 * @return true io complete
		 * @return false io incomplete
		 */
		virtual bool complete(const descriptor_type&) { return true; }

		/**
		 * @brief call when io complete
		 *
		 */
		virtual void operator()(void) = 0;
	public:
		std::error_code error(void) { return _error; }
		int size(void) { return _size; }
		void set(const std::error_code& ec, const int sz) {
			_error = ec;
			_size = sz;
		}
	private:
		std::error_code _error;
		int _size;
		/*
#if defined(CX_PLATFORM_WIN32)
		std::error_code error(void) {
			return std::error_code(static_cast<int>(ov.Internal),
				*reinterpret_cast<std::error_category*>(ov.InternalHigh));
		}
		int size(void) { return ov.Offset; }
		void set(const std::error_code& ec, const int sz) {
			ov.Internal = ec.value();
			ov.InternalHigh = reinterpret_cast<ULONG_PTR>(&(ec.category()));
			ov.Offset = sz;
		}

		OVERLAPPED* overlapped(void) {
			memset(&ov, 0x00, sizeof(ov));
			return &ov;
		}
	public:
		OVERLAPPED ov;
#else
		std::error_code error(void) { return _error; }
		int size(void) { return _size; }
		void set(const std::error_code& ec, const int sz) {
			_error = ec;
			_size = sz;
		}
	private:
		std::error_code _error;
		int _size;
#endif*/
	};
}
#endif