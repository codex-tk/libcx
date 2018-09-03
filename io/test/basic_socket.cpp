/**
 * @brief 
 * 
 * @file basic_socket.cpp
 * @author ghtak
 * @date 2018-08-27
 */

#include <gtest/gtest.h>
#include <cx/base/defines.hpp>

#if defined(CX_PLATFORM_WIN32)

TEST(win32, handle_types) {
	ASSERT_EQ(-1, INVALID_SOCKET);
	ASSERT_EQ(-1, (int)INVALID_HANDLE_VALUE);
}

#endif