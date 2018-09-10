/**
 * @brief 
 * 
 * @file basic_stream_service.hpp
 * @author ghtak
 * @date 2018-09-01
 */
#ifndef __cx_io_ip_basic_stream_service_h__
#define __cx_io_ip_basic_stream_service_h__

#include <cx/base/defines.hpp>
#include <cx/base/error.hpp>
#include <cx/io/io.hpp>
#include <cx/io/ip/basic_address.hpp>
#include <cx/io/ip/services/basic_service.hpp>
#include <cx/io/ops/basic_io_operation.hpp>
#include <cx/io/ops/basic_read_operation.hpp>
#include <cx/io/ops/basic_write_operation.hpp>
#include <cx/io/ops/handler_operation.hpp>
namespace cx::io::ip {

    template < typename EngineType, int Type = SOCK_STREAM, int Proto = IPPROTO_TCP > 
	class basic_stream_service {
    public:
		using this_type = basic_stream_service;
		using engine_type = EngineType;
		using mux_type = typename engine_type::mux_type;
		using operation_type = typename mux_type::operation_type;
		using descriptor_type = typename mux_type::descriptor_type;
		using basic_service_type = cx::io::ip::basic_service<engine_type>;
		using address_type = cx::io::ip::basic_address<Type, Proto>;
		using buffer_type = cx::io::buffer;

		using read_operation = cx::io::basic_read_operation<this_type, cx::io::basic_io_operation<this_type>>;
		using write_operation = cx::io::basic_write_operation<this_type, cx::io::basic_io_operation<this_type>>;

		template <typename HandlerType>
		static void read(
			const descriptor_type& descriptor,
			const buffer_type& buf,
			HandlerType&& handler)
		{
			read_operation* op = new dgram_handler_operation<read_operation, HandlerType>(
				std::forward<HandlerType>(handler));
			op->buffer().reset(buf.base(), buf.length());
			if (!mux_type::good(descriptor)) {
				auto ops = mux_type::drain_ops(descriptor, std::make_error_code(std::errc::bad_file_descriptor));
				op->set(std::make_error_code(std::errc::invalid_argument), 0);
				ops.add_tail(op);
				descriptor->engine.post(std::move(ops));
				return;
			}
			bool request = descriptor->context[0].ops.empty();
			descriptor->context[0].ops.add_tail(op);
			if (request) {
				int ops = cx::io::pollin | (descriptor->context[1].ops.empty() ? 0 : cx::io::pollout);
				if (!descriptor->engine.multiplexer().bind(descriptor, ops)) {
					descriptor->engine.post(mux_type::drain_ops(descriptor, cx::system_error()));
				}
			}
		}

		template <typename HandlerType>
		static void write(
			const descriptor_type& descriptor,
			const buffer_type& buf,
			HandlerType&& handler)
		{
			write_operation* op = new dgram_handler_operation<write_operation, HandlerType>(
				std::forward<HandlerType>(handler));
			op->buffer().reset(buf.base(), buf.length());
			if (!mux_type::good(descriptor)) {
				auto ops = mux_type::drain_ops(descriptor, std::make_error_code(std::errc::bad_file_descriptor));
				op->set(std::make_error_code(std::errc::invalid_argument), 0);
				ops.add_tail(op);
				descriptor->engine.post(std::move(ops));
				return;
			}
			bool request = descriptor->context[1].ops.empty();
			descriptor->context[1].ops.add_tail(op);
			if (request) {
				int ops = cx::io::pollout | (descriptor->context[0].ops.empty() ? 0 : cx::io::pollin);
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
			int ret = basic_service_type::recv(descriptor,
				op->buffer().base(),
				op->buffer().length(),
				0,
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
			int ret = basic_service_type::send(descriptor,
				op->buffer().base(),
				op->buffer().length(),
				0,
				ec);
			op->set(ec, ret);
			return true;
		}
    };
}

#endif