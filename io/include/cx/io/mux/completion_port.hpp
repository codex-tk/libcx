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
#include <cx/io/io.hpp>
#include <cx/io/basic_operation.hpp>
#include <iostream>

#if defined(CX_PLATFORM_WIN32)

namespace cx::io {
	template < typename MuxT >
	class basic_engine;
}

namespace cx::io::mux {

	template <typename DescriptorType>
	struct OVERLAPPEDEX : OVERLAPPED { 
		DescriptorType descriptor;
		cx::io::type type;

		OVERLAPPED* ptr(void) {
			memset(static_cast<OVERLAPPED*>(this), 0x00, sizeof(OVERLAPPED));
			return static_cast<OVERLAPPED*>(this);
		}
	};

	template <typename DescriptorType, typename OperationType>
	struct OVERLAPPEDEX_OP : OVERLAPPEDEX<DescriptorType> {
		OVERLAPPEDEX_OP(void) { type = cx::io::pollop; }
		OperationType* operation;
	};

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

		using socket_type = SOCKET;

		struct descriptor :
			private cx::noncopyable,
			public std::enable_shared_from_this<descriptor>
		{
			union {
				socket_type s;
				HANDLE h;
			} fd;
			basic_engine<this_type>& engine;

			OVERLAPPEDEX<descriptor_type> overlapped[2];
			cx::slist<operation_type> ops[2];
			
			descriptor(basic_engine<this_type>& e);
			~descriptor(void) {
				std::cout << "~descriptor" << std::endl;
			}
		};

		static const socket_type invalid_socket = INVALID_SOCKET;

		static socket_type socket_handle(const completion_port::descriptor_type& descriptor);
		static socket_type socket_handle(const completion_port::descriptor_type& descriptor
			, socket_type s);

		static bool good(const completion_port::descriptor_type& descriptor) {
			return socket_handle(descriptor) != invalid_socket;
		}

		static cx::slist<operation_type> drain_ops(
			const completion_port::descriptor_type& descriptor,
			const std::error_code& ec
		);

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
		int handle_event(const descriptor_type& descriptor, int type,
			const std::error_code& ec, DWORD byte_transferred);
		int handle_event(const descriptor_type& descriptor, operation_type* op,
			const std::error_code& ec, DWORD byte_transferred);
	private:
		basic_engine<this_type>& _engine;
		HANDLE _handle;
	};


}

#endif

#endif
