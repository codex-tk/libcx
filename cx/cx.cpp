#include <iostream>
#include <cx/cx.hpp>
#include <cx/error.hpp>

namespace cx {
    std::string say_hello(void){
        return std::string( "hello cx" );
    }
#if CX_PLATFORM ==  CX_P_WINDOWS
	namespace internal {
		std::error_category& windows_category(void) {
			static internal::windows_category_impl impl;
			return impl;
		}
	}

	std::error_code get_last_error(void) {
		return std::error_code(WSAGetLastError(), internal::windows_category());
	}
#else
	std::error_code get_last_error(void) {
		return std::error_code(errno, std::generic_category());
	}
#endif

	std::error_code make_error_code(cx::errc ec) {
		return std::error_code(static_cast<int>(ec), cx::internal::cx_error_category::instance());
	}
}
