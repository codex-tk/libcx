/**
 * @brief 
 * 
 * @file basic_acceptor.hpp
 * @author ghtak
 * @date 2018-09-13
 */
#ifndef __cx_io_ip_basic_acceptor_h__
#define __cx_io_ip_basic_acceptor_h__

#include <cx/base/defines.hpp>
#include <cx/io/io.hpp>
#include <cx/io/ip/option.hpp>
#include <cx/io/ip/basic_socket.hpp>

namespace cx::io::ip {

	/**
	 * @brief 
	 * 
	 * @tparam EngineType 
	 * @tparam ServiceType 
	 */
	template <typename EngineType, typename ServiceType> class basic_acceptor {
	public:
		using engine_type = EngineType;
		using service_type = ServiceType;

		using mux_type = typename service_type::mux_type;
		using descriptor_type = typename service_type::descriptor_type;
		using basic_service_type = typename service_type::basic_service_type;
		using address_type = typename service_type::address_type;
		using socket_type = typename mux_type::socket_type;

		basic_acceptor(engine_type& e)
			: _descriptor(std::make_shared<
				typename descriptor_type::element_type>(e)) {
		}

		bool open(const address_type& addr,
			std::error_code& ec)
		{
			if (!basic_service_type::open(_descriptor, addr, ec))
				return false;
			
			cx::io::ip::option::reuse_address reuse_addr;
			if (!reuse_addr.set(mux_type::socket_handle(_descriptor)))
				return false;

			if (!_descriptor->engine.multiplexer().bind(_descriptor, ec))
				return false;

			if (!basic_service_type::bind(_descriptor, addr, ec))
				return false;

			if (!basic_service_type::listen(_descriptor, SOMAXCONN, ec))
				return false;

			return true;
		}

		void close(void) {
			basic_service_type::close(_descriptor);
			auto ops = mux_type::drain_ops(_descriptor, std::error_code());
			if (!ops.empty())
				_descriptor->engine.post(std::move(ops));
		}

		bool accept(cx::io::ip::basic_socket<engine_type, service_type>& fd, 
			address_type& addr, 
			std::error_code& ec) 
		{
			if (basic_service_type::accept(_descriptor,
				fd->descriptor(),
				addr.sockaddr(),
				addr.length_ptr(),
				ec))
			{
				if (fd->descriptor()->engine.multiplexer().bind(_descriptor, ec)) {
					return true;
				}
			}
			fd->close();
			return false;
		}

		template <typename HandlerType>
		void async_accept(cx::io::ip::basic_socket<engine_type, service_type>& fd, HandlerType&& handler) {
			service_type::accept(_descriptor, fd.descriptor(), std::forward<HandlerType>(handler));
		}

		bool poll(const std::chrono::milliseconds& ms) {
			std::error_code ec;
			return basic_service_type::poll(_descriptor, cx::io::pollin, ms, ec) == cx::io::pollin;
		}
	private:
		descriptor_type _descriptor;
	};

}

#endif