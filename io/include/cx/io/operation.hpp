/**
 * @brief
 *
 * @file operation.hpp
 * @author ghtak
 * @date 2018-08-26
 */
#ifndef __cx_io_operation_h__
#define __cx_io_operation_h__

#include <cx/base/defines.hpp>
#include <cx/base/noncopyable.hpp>
#include <cx/base/slist.hpp>

#include <cx/io/descriptor.hpp>

namespace cx::io {

	/**
	 * @brief
	 *
	 */
	class operation :
		public cx::noncopyable,
		public cx::slist<operation>::node {
	public:
		operation(void);
		virtual ~operation(void) = default;

		virtual bool is_complete(const cx::io::descriptor_t&) = 0;

		virtual void operator()(void) = 0;
	public:
		std::error_code error(void);
		int size(void);

		void set(const std::error_code& ec, const int sz);
#if defined(CX_PLATFORM_WIN32)
	public:
		int type(void);
		OVERLAPPED* reset_overlapped(int type);
	private:
		struct OVERLAPPEDEX : OVERLAPPED { 
			int type; 
		} _overlapped;
#else
	private:
		std::error_code _error;
		int _size;
#endif
	};

}

#endif