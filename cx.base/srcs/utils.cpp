#include <cx/base/utils.hpp>

#ifdef __GNUG__
	#include <cstdlib>
	#include <memory>
	#include <cxxabi.h>
#else
	#if defined(CX_PLATFORM_WIN32)
		#include <Dbghelp.h>
		#pragma comment(lib,"dbghelp.lib")    
	#endif
#endif

namespace cx::base {

	uint16_t checksum(void* p, int sz) {
		assert(p != nullptr);
		assert(sz >= 0);
		int32_t sum = 0;
		if (p != nullptr) {
			uint16_t *ptr = static_cast<uint16_t *>(p);
			for (; sz > 1; sz -= 2) {
				sum += *ptr;
				++ptr;
			}
			if (sz == 1)
				sum += static_cast<uint16_t>(*reinterpret_cast<uint8_t *>(ptr));
		}
		sum = (sum >> 16) + (sum & 0xffff);
		sum += (sum >> 16);
		return static_cast<uint16_t>(~sum);
	}

	std::string demangle(const char *name) {
#ifdef __GNUG__        
		int status = -4; 
		// some arbitrary value to eliminate the compiler warning
		// enable c++11 by passing the flag -std=c++11 to g++
		std::unique_ptr<char, void(*)(void *)> res{
			abi::__cxa_demangle(name, NULL, NULL, &status),
			std::free };
		return (status == 0) ? res.get() : name;
#else
	#if defined(CX_PLATFORM_WIN32)
		/*
		SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
		SymInitialize(GetCurrentProcess(), NULL, TRUE);
		char undecorated_name[1024];
		if (!UnDecorateSymbolName(name, undecorated_name, sizeof(undecorated_name) / sizeof(*undecorated_name), UNDNAME_COMPLETE))
		{
			return undecorated_name;
		}
		*/
	#endif
		return name;
#endif    
	}
}