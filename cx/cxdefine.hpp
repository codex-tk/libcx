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

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <windows.h>
#include <stdint.h>

#pragma comment( lib , "ws2_32.lib")
#pragma comment( lib , "Mswsock.lib") 
#pragma comment( lib , "IPHLPAPI.lib")

namespace cx::io::ip::detail{
    namespace {
        struct win32_socket_initializer {
            win32_socket_initializer(void){
                WSADATA    wsaData;
                WSAStartup(MAKEWORD(2,2), &wsaData);
            }
        };
        static win32_socket_initializer initializer;
    }
}

#else

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

#endif
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#include <string>
#include <system_error>
#include <chrono>

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#endif