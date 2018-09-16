/**
 * @brief 
 * 
 * @file basic_dgram_service.hpp
 * @author ghtak
 * @date 2018-09-01
 */
#ifndef __cx_io_ip_basic_dgram_service_h__
#define __cx_io_ip_basic_dgram_service_h__

#include <cx/base/defines.hpp>
#include <cx/base/error.hpp>
#include <cx/io/buffer.hpp>
#include <cx/io/ip/basic_address.hpp>
#include <cx/io/ip/services/basic_service.hpp>
#include <cx/io/ops/basic_dgram_operation.hpp>
#include <cx/io/ops/basic_read_operation.hpp>
#include <cx/io/ops/basic_write_operation.hpp>
#include <cx/io/ops/handler_operation.hpp>

namespace cx::io::ip {

	template < typename EngineType, int Type = SOCK_DGRAM , int Proto = IPPROTO_UDP > 
	class basic_dgram_service {
	public:
		using this_type = basic_dgram_service;
		using engine_type = EngineType;
		using mux_type = typename engine_type::mux_type;
		using operation_type = typename mux_type::operation_type;
		using descriptor_type = typename mux_type::descriptor_type;
		using basic_service_type = cx::io::ip::basic_service<engine_type>;
		using address_type = cx::io::ip::basic_address<Type, Proto>;
		using buffer_type = cx::io::buffer;

		using read_operation = cx::io::basic_read_operation<this_type, cx::io::ip::basic_dgram_operation<this_type>>;
		using write_operation = cx::io::basic_write_operation<this_type, cx::io::ip::basic_dgram_operation<this_type>>;

		template <typename HandlerType>
		static void read(
			const descriptor_type& descriptor,
			const buffer_type& buf,
			const address_type& addr,
			HandlerType&& handler)
		{
			read_operation* op = new dgram_handler_operation<read_operation, HandlerType>(
				std::forward<HandlerType>(handler));
			op->buffer().reset(buf.base(), buf.length());
			op->address() = addr;
			if (!mux_type::good(descriptor)) {
				auto ops = mux_type::drain_ops(descriptor, std::make_error_code(std::errc::bad_file_descriptor));
				op->set(std::make_error_code(std::errc::invalid_argument), 0);
				ops.add_tail(op);
				descriptor->engine.post(std::move(ops));
				return;
			}
			bool request = descriptor->ops[0].empty();
			descriptor->ops[0].add_tail(op);
			if (request) {
				int ops = cx::io::pollin | (descriptor->ops[1].empty() ? 0 : cx::io::pollout);
				if (!descriptor->engine.multiplexer().bind(descriptor, ops)) {
					descriptor->engine.post(mux_type::drain_ops(descriptor, cx::system_error()));
				}
			}
		}

		template <typename HandlerType>
		static void write(
			const descriptor_type& descriptor,
			const buffer_type& buf,
			const address_type& addr,
			HandlerType&& handler)
		{
			write_operation* op = new dgram_handler_operation<write_operation, HandlerType>(
				std::forward<HandlerType>(handler));
			op->buffer().reset(buf.base(), buf.length());
			op->address() = addr;
			if (!mux_type::good(descriptor)) {
				auto ops = mux_type::drain_ops(descriptor, std::make_error_code(std::errc::bad_file_descriptor));
				op->set(std::make_error_code(std::errc::invalid_argument), 0);
				ops.add_tail(op);
				descriptor->engine.post(std::move(ops));
				return;
			}
			bool request = descriptor->ops[1].empty();
			descriptor->ops[1].add_tail(op);
			if (request) {
				int ops = cx::io::pollout | (descriptor->ops[0].empty() ? 0 : cx::io::pollin);
				if (!descriptor->engine.multiplexer().bind(descriptor, ops)) {
					descriptor->engine.post(mux_type::drain_ops(descriptor, cx::system_error()));
				}
			}
		}

		static void read_request(const descriptor_type&, read_operation*) {}

		static bool read_complete(
			const descriptor_type& descriptor,
			read_operation* op)
		{
			std::error_code ec;
			int ret = basic_service_type::recvfrom(descriptor, 
				op->buffer().base(), 
				op->buffer().length(),
				0, 
				op->address().sockaddr(),
				op->address().length_ptr(),
				ec);
			op->set(ec, ret);
			return true;
		}

		static void write_request(const descriptor_type&, write_operation*) {}

		static bool write_complete(
			const descriptor_type& descriptor,
			write_operation* op)
		{
			std::error_code ec;
			int ret = basic_service_type::sendto(descriptor,
				op->buffer().base(),
				op->buffer().length(),
				0,
				op->address().sockaddr(),
				op->address().length(),
				ec);
			op->set(ec, ret);
			return true;
		}
	};
}
#endif