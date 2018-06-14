/**
 */
#ifndef __cx_error_h__
#define __cx_error_h__

#include <cx/cxdefine.hpp>
#include <map>

namespace cx {
	enum class errc {
		success = 0 ,
		operation_canceled = 1,
	};
#if CX_PLATFORM == CX_P_WINDOWS
	class windows_category_impl : public std::error_category {
	public:
		//! construct
		windows_category_impl(void) {}

		//! destruct
		virtual ~windows_category_impl() noexcept {}

		//! \return "tcode_category"
		virtual const char *name() const noexcept
		{
			return "windows_category";
		}

		//! \return readable error message­
		//! \param condition [in] 
		virtual std::string message(int condition) const {
			const size_t buffer_size = 4096;
			DWORD dwFlags = FORMAT_MESSAGE_FROM_SYSTEM;
			LPCVOID lpSource = NULL;
			char buffer[buffer_size] = { 0, };
			unsigned long result;
			result = ::FormatMessageA(
				dwFlags,
				lpSource,
				condition,
				MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
				buffer,
				buffer_size,
				NULL);

			if (result == 0) {
				std::ostringstream os;
				os << "unknown error code: " << condition << ".";
				return os.str();
			}
			char* pos = strrchr(buffer, '\r');
			if (pos)
				*pos = '\0';
			return std::string(buffer);
		}
	};

	std::error_category& windows_category(void);
#endif
	std::error_code get_last_error(void);
}

namespace std {
	//template <> struct is_error_condition_enum<cx::errc> : true_type {};
	template <> struct is_error_code_enum<cx::errc> : true_type {};
}

namespace cx {
	namespace internal {
		class cx_error_category : public ::std::error_category {
		public:
			cx_error_category(void) {
				_error_message[cx::errc::success] = "success";
				_error_message[cx::errc::operation_canceled] = "operation_canceled";
			}

			virtual const char *name() const noexcept override {
				return "cx_error_category";
			}

			virtual ::std::string message(int code) const override {
				auto it = _error_message.find(static_cast<cx::errc>(code));
				if (it != _error_message.end())
					return it->second;
				return std::string();
			}
			
			virtual bool equivalent(int code
				, const ::std::error_condition& cond) const noexcept override 
			{
				if (cond.value() == 0 && code == 0) return true;
				if (cond.category() == std::generic_category()) {
					switch (static_cast<std::errc>(cond.value())) {
					case ::std::errc::operation_canceled:
						return static_cast<cx::errc>(code) == cx::errc::operation_canceled;
					default:
						break;
					}
				}
				return false;
			}
			/*
			virtual bool equivalent(const std::error_code& ec, int errval) const {
				switch (static_cast<cx::errc>(errval)) {
				case cx::errc::success:
					if (!ec) return true;
					break;
				}
				return false;
			}
			*/
			static cx_error_category& instance(void) {
				static cx_error_category category;
				return category;
			}
		private:
			::std::map<cx::errc, std::string> _error_message;
		};
	}

	std::error_code make_error_code(cx::errc ec);
}



#endif