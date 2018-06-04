/**
 */
#ifndef __cx_error_h__
#define __cx_error_h__

#include <cx/cxdefine.hpp>

namespace cx {

#if CX_PLATFORM == CX_P_WINDOWS
    class windows_category_impl : public std::error_category {
    public:
        //! construct
        windows_category_impl(){
        }

        //! destruct
        virtual ~windows_category_impl() noexcept {
        }

        //! \return "tcode_category"
        virtual const char *name() const noexcept
        {
            return "windows_category";
        }

        //! \return readable error message­
        //! \param condition [in] 
        virtual std::string message( int condition ) const {
            const size_t buffer_size = 4096;
			DWORD dwFlags = FORMAT_MESSAGE_FROM_SYSTEM;
			LPCVOID lpSource = NULL;
			char buffer[buffer_size] = {0,};
			unsigned long result;
			result = ::FormatMessageA(
				dwFlags,
				lpSource,
				condition,
				MAKELANGID( LANG_ENGLISH , SUBLANG_ENGLISH_US ),
				buffer,
				buffer_size,
				NULL);

			if (result == 0) {
				std::ostringstream os;
				os << "unknown error code: " << condition << ".";
				return os.str();
			} 
			char* pos = strrchr( buffer , '\r' );
			if ( pos ) 
				*pos = '\0';
			return std::string( buffer );
        }
    };

	static std::error_category& windows_category() {
		static windows_category_impl impl;
		return impl;
	}
#endif
}


#endif