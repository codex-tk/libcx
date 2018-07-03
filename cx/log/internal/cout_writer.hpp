/**
 */
#ifndef __cx_log_cout_writer_h__
#define __cx_log_cout_writer_h__

#include <cx/log/writer.hpp>

namespace cx::log {

	class cout_writer : public cx::log::writer {
	public:
#if CX_PLATFORM != CX_P_WINDOWS            
		enum code {
			black = 30,
			red = 31,
			green = 32,
			yellow = 33,
			blue = 34,
			magenta = 35,
			cyan = 36,
			white = 37,
		};
#endif
		virtual void operator()(const cx::log::record& r, const cx::basic_buffer<char>& formatted) {
#if CX_PLATFORM != CX_P_WINDOWS            
			int colors[] = { 32 , 33, 36 , 35 , 31 , 34 };
			std::cout << "\033[" << colors[static_cast<int>(r.level)] << "m" << formatted.rdptr() << std::endl;
#else
			WORD colors[7] = {
				FOREGROUND_GREEN ,
				FOREGROUND_GREEN | FOREGROUND_RED ,
				FOREGROUND_BLUE | FOREGROUND_GREEN ,
				FOREGROUND_BLUE | FOREGROUND_RED ,
				FOREGROUND_RED , 
				 FOREGROUND_BLUE ,
				FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
			};
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colors[static_cast<int>(r.level)]);
			std::cout << formatted.rdptr() << std::endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colors[6]);
#endif
		}
	public:
		static std::shared_ptr<writer> instance(void) {
			static std::shared_ptr<writer> writer_ptr = std::make_shared<cout_writer>();
			return writer_ptr;
		}
	};
}

#endif