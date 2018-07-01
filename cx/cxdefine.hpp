#ifndef __cx_define_h__
#define __cx_define_h__

/*
Darwin	 DARWIN						Darwin is not necessarily OS X, see below
FreeBSD	 __FreeBSD__				Consider using the BSD macro
Linux	 linux or __linux			I also found LINUX
MacOS X	 __MACOSX__ or __APPLE__
NetBSD	 __NetBSD__					Consider using the BSD macro
OpenBSD	 __OpenBSD__				Consider using the BSD macro
Solaris	 sun or __sun				SunOS versions < 5 will not have __SVR4 or __svr4__ defined
Windows	 _WIN32 or __WIN32__
*/

#define CX_P_LINUX 0
#define CX_P_WINDOWS 1
#define CX_P_MACOSX 2
#define CX_P_UNKNOWN 0xffffffff

#if defined( linux ) || defined ( __linux )
#define CX_PLATFORM CX_P_LINUX
#elif defined( _WIN32 ) || defined (_WIN64) 
#define CX_PLATFORM CX_P_WINDOWS
#elif defined ( __MACOSX__ ) || defined ( __APPLE__ ) 
#define CX_PLATFORM CX_P_MACOSX
#else
#define CX_PLATFORM CX_P_UNKNOWN
#endif


#if CX_PLATFORM == CX_P_WINDOWS

#if !defined(NOMINMAX)
#define NOMINMAX
#endif

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <MSWsock.h>
#include <iphlpapi.h>
#include <windows.h>
#include <stdint.h>

#pragma comment( lib , "ws2_32.lib")
#pragma comment( lib , "Mswsock.lib") 
#pragma comment( lib , "IPHLPAPI.lib")

namespace cx::io::ip::internal {
	namespace {
		struct win32_socket_initializer {
			win32_socket_initializer(void) {
				WSADATA    wsaData;
				WSAStartup(MAKEWORD(2, 2), &wsaData);
			}
		};
		static win32_socket_initializer initializer;
	}
}

#define CX_LITTLE_ENDIAN

#else 

//https://stackoverflow.com/questions/4239993/determining-endianness-at-compile-time/4240029
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
// It's a big-endian target architecture
#define CX_BIG_ENDIAN
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
// It's a little-endian target architecture
#define CX_LITTLE_ENDIAN
#else
//error "I don't know what architecture this is!"
#define CX_LITTLE_ENDIAN
#endif



#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#if CX_PLATFORM == CX_P_LINUX

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <netinet/tcp.h>

#endif // CX_P_LINUX

#endif // CX_P_WINDOWS 

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cassert>

#include <string>
#include <system_error>
#include <chrono>
#include <queue>
#include <array>
#include <atomic>
#include <memory>
#include <functional>
#include <cassert>
#include <sstream>
#include <mutex>
#include <thread>
#include <time.h>


#define _USE_MATH_DEFINES

#include <math.h>

#ifndef MAX_PATH
#define MAX_PATH 256
#endif



#endif
