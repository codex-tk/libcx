/**
 * @brief 
 * 
 * @file gprintf.cpp
 * @author ghtak
 * @date 2018-09-11
 */
#include "gprintf.hpp"

void gprintf(const char* fmt, ...) {
	printf("[   USER   ] ");
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("\n");
}