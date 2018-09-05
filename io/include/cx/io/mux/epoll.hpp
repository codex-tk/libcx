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
		using this_type = epoll;

		struct descriptor;

		using descriptor_type = std::shared_ptr<descriptor>;
		using operation_type = basic_operation<descriptor_type>;

		struct descriptor :
			private cx::noncopyable,
			public std::enable_shared_from_this<descriptor>
		{
			int fd;
			struct {
				cx::slist<operation_type> ops;
			} context[2];

			descriptor(void);
			int socket_handle(void) { return fd; }
			int socket_handle(int s) {
				std::swap(s, fd);
				return s;
			}
			int file_handle(void) { return fd; }
			int file_handle(int h) {
				std::swap(h, fd);
				return h;
			}
		};

		epoll(basic_engine<this_type>& e);

		~epoll(void);


		bool bind(const descriptor_type& descriptor);

		bool bind(const descriptor_type& descriptor, std::error_code& ec);

		bool bind(const descriptor_type& fd, int ops);

		bool bind(const descriptor_type& fd, int ops, std::error_code& ec);

		void unbind(const descriptor_type& fd);

		void wakeup(void);

		int run(const std::chrono::milliseconds& wait_ms);

	private:
		basic_engine<this_type>& _engine;
		int _handle;
		int _eventfd;
	};
}

#endif

#endif