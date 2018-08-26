/**
 * @brief 
 * 
 * @file operation.cpp
 * @author ghtak
 * @date 2018-08-26
 */

#include <cx/io/operation.hpp>

namespace cx::io {

	operation::operation(void) {
#if defined(CX_PLATFORM_WIN32)
		reset_overlapped(0);
#endif
	}

	std::error_code operation::error(void) {
#if defined(CX_PLATFORM_WIN32)
		return std::error_code(
			_overlapped.Internal,
			*static_cast<std::error_category*>(_overlapped.Pointer)
		);
#else
		return _error;
#endif
	}

	int operation::size(void) {
#if defined(CX_PLATFORM_WIN32)
		return static_cast<int>(_overlapped.InternalHigh);
#else
		return _size;
#endif
	}

	void operation::set(const std::error_code& ec, const int sz) {
#if defined(CX_PLATFORM_WIN32)
		_overlapped.Internal = ec.value();
		_overlapped.Pointer = const_cast< std::error_category*>(&(ec.category()));
		_overlapped.InternalHigh = sz;
#else
		_error = ec;
		_size = sz;
#endif
	}

#if defined(CX_PLATFORM_WIN32)
	int operation::type(void) {
		return _overlapped.type;
	}

	OVERLAPPED* operation::reset_overlapped(int type) {
		memset(&_overlapped, 0x00, sizeof(_overlapped));
		_overlapped.type = type;
		return &_overlapped;
	}
#endif

}