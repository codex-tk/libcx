/**
 * @brief 
 * 
 * @file completion_port.hpp
 * @author ghtak
 * @date 2018-08-31
 */
#ifndef __cx_io_completion_port_h__
#define __cx_io_completion_port_h__

#include <cx/base/defines.hpp>
#include <cx/io/basic_operation.hpp>

#if defined(CX_PLATFORM_WIN32)

namespace cx::io {
	template < typename MuxT >
	class basic_engine;
}

namespace cx::io::mux {

	/**
	* @brief
	*
	*/
	class completion_port
		: private cx::noncopyable {
	public:
		using this_type = completion_port;

		struct descriptor;

		using descriptor_type = std::shared_ptr<descriptor>;
		using operation_type = basic_operation<descriptor_type>;

		struct descriptor :
			private cx::noncopyable,
			public std::enable_shared_from_this<descriptor>
		{
			struct OVERLAPPEDEX : OVERLAPPED { int type; };
			union {
				SOCKET s;
				HANDLE h;
			} fd;
			struct {
				cx::slist<operation_type> ops;
				OVERLAPPEDEX overlapped;
			} context[2];
			descriptor(void);
			
			SOCKET socket_handle(void) { return fd.s; }
			SOCKET socket_handle(SOCKET s) {
				std::swap(s, fd.s);
				return s;
			}
			HANDLE file_handle(void) { return fd.h; }
			HANDLE file_handle(HANDLE h) {
				std::swap(h, fd.h);
				return h;
			}
		};

		completion_port(basic_engine<this_type>& e);

		~completion_port(void);

		bool bind(const descriptor_type& descriptor);

		bool bind(const descriptor_type& descriptor, std::error_code& ec);

		bool bind(const descriptor_type& descriptor, int ops);

		bool bind(const descriptor_type& descriptor, int ops, std::error_code& ec);

		void unbind(const descriptor_type& descriptor);

		void wakeup(void);

		int run(const std::chrono::milliseconds& wait_ms);
	private:
		basic_engine<this_type>& _engine;
		HANDLE _handle;
	};
}

#endif

#endif