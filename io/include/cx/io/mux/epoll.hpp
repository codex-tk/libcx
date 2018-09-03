/**
 * @brief 
 * 
 * @file epoll.hpp
 * @author ghtak
 * @date 2018-08-26
 */
#ifndef __cx_io_epoll_h__
#define __cx_io_epoll_h__

#include <cx/base/defines.hpp>
#include <cx/io/io.hpp>
#include <cx/io/basic_operation.hpp>

#if defined(CX_PLATFORM_LINUX)

namespace cx::io {
	template < typename MuxT >
	class basic_engine;
}

namespace cx::io::mux {

	/**
	* @brief
	*
	*/
	class epoll
		: private cx::noncopyable {
	public:
		struct descriptor;

		using descriptor_ptr = std::shared_ptr<descriptor>;
		using operation = basic_operation<descriptor_ptr>;

		struct descriptor :
			private cx::noncopyable,
			public std::enable_shared_from_this<descriptor>
		{
			int fd;
			struct {
				cx::slist<operation> ops;
			} context[2];
		};

		epoll(basic_engine<epoll>* e);

		~epoll(void);


		bool bind(const descriptor_ptr& descriptor);

		bool bind(const descriptor_ptr& descriptor, std::error_code& ec);

		bool bind(const descriptor_ptr& fd, int ops);

		bool bind(const descriptor_ptr& fd, int ops, std::error_code& ec);

		void unbind(const descriptor_ptr& fd);

		void wakeup(void);

		int run(const std::chrono::milliseconds& wait_ms);

	private:
		basic_engine<epoll>* _engine;
		int _handle;
		int _eventfd;
	};
}

#endif

#endif