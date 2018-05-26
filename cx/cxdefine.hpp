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

#endif

