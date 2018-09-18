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
#include <iostream>

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

		using socket_type = int;

		struct descriptor :
			private cx::noncopyable,
			public std::enable_shared_from_this<descriptor>
		{
			socket_type fd;
			cx::slist<operation_type> ops[2];
			basic_engine<this_type>& engine;
			descriptor(basic_engine<this_type>& e);
		};

		static const socket_type invalid_socket = -1;

		static socket_type socket_handle(const epoll::descriptor_type& descriptor);
		static socket_type socket_handle(const epoll::descriptor_type& descriptor
			, socket_type s);

		static bool good(const epoll::descriptor_type& descriptor) {
			return socket_handle(descriptor) != invalid_socket;
		}

		static cx::slist<operation_type> drain_ops(const epoll::descriptor_type& descriptor,
			const std::error_code& ec);

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
		int handle_event(struct epoll_event& ev);
	private:
		basic_engine<this_type>& _engine;
		int _handle;
		int _eventfd;
	};

}

#endif

#endif