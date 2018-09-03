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
		struct descriptor;

		using descriptor_ptr = std::shared_ptr<descriptor>;
		using operation = basic_operation<descriptor_ptr>;

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
				cx::slist<operation> ops;
				OVERLAPPEDEX overlapped;
			} context[2];
		};

		completion_port(basic_engine<completion_port>* e);

		~completion_port(void);

		bool bind(const descriptor_ptr& descriptor);

		bool bind(const descriptor_ptr& descriptor, std::error_code& ec);

		bool bind(const descriptor_ptr& descriptor, int ops);

		bool bind(const descriptor_ptr& descriptor, int ops, std::error_code& ec);

		void unbind(const descriptor_ptr& descriptor);

		void wakeup(void);

		int run(const std::chrono::milliseconds& wait_ms);
	private:
		basic_engine<completion_port>* _engine;
		HANDLE _handle;
	};
}

#endif

#endif