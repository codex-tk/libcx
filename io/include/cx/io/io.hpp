/**
 * @brief 
 * 
 * @file io.hpp
 * @author ghtak
 * @date 2018-08-26
 */
#ifndef __cx_io_io_h__
#define __cx_io_io_h__

#include <cx/base/defines.hpp>

namespace cx::io {

	/*
	#define POLLIN      0x0001  // ���� ����Ÿ�� �ִ�.
	#define POLLPRI     0x0002  // ����� ���� ����Ÿ�� �ִ�.
	#define POLLOUT     0x0004  // ���Ⱑ ����(block)�� �ƴϴ�.
	#define POLLERR     0x0008  // �����߻�
	#define POLLHUP     0x0010  // ������ ������
	#define POLLNVAL    0x0020  // ���������ڰ� ������ �����Ͱ���
	// Invalid request (�߸��� ��û)

	enum EPOLL_EVENTS
	{
	EPOLLIN = 0x001,
	#define EPOLLIN EPOLLIN
	EPOLLPRI = 0x002,
	#define EPOLLPRI EPOLLPRI
	EPOLLOUT = 0x004,
	#define EPOLLOUT EPOLLOUT
	EPOLLRDNORM = 0x040,
	#define EPOLLRDNORM EPOLLRDNORM
	EPOLLRDBAND = 0x080,
	#define EPOLLRDBAND EPOLLRDBAND
	EPOLLWRNORM = 0x100,
	#define EPOLLWRNORM EPOLLWRNORM
	EPOLLWRBAND = 0x200,
	#define EPOLLWRBAND EPOLLWRBAND
	EPOLLMSG = 0x400,
	#define EPOLLMSG EPOLLMSG
	EPOLLERR = 0x008,
	#define EPOLLERR EPOLLERR
	EPOLLHUP = 0x010,
	#define EPOLLHUP EPOLLHUP
	EPOLLET = (1 << 31)
	#define EPOLLET EPOLLET
	};
	*/
	enum type {
		pollin = 0x01,
		pollout = 0x04,
	};
}


#endif