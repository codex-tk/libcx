/**
 * @brief 
 * 
 * @file basic_stream_service.hpp
 * @author ghtak
 * @date 2018-09-01
 */
#ifndef __cx_io_ip_basic_stream_service_h__
#define __cx_io_ip_basic_stream_service_h__

#include <cx/base/error.hpp>
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

		using read_operation = cx::io::basic_read_operation< cx::io::basic_io_operation<this_type>>;
		using write_operator = cx::io::basic_write_operation< cx::io::basic_io_operation<this_type>>;

		template <typename HandlerType>
		static void send(engine_type& engine,
			const descriptor_type& descriptor,
			buffer_type& buf,
			HandlerType&& handler)
		{
			read_operation* op = new handler_operation<read_operation, HandlerType>(
				std::forward<HandlerType>(handler));
			if (!mux_type::good(descriptor)) {
				engine.post(mux_type::drain_ops(descriptor), 
					std::make_error_code(std::errc::bad_file_descriptor));
				op->error(std::make_error_code(std::errc::invalid_argument));
				engine.post(op);
				return;
			}
			int ops = 0;
			if (descriptor->context[0].ops.empty()) {
				ops = cx::io::pollin | (descriptor->context[1].ops.empty() ? 0 : cx::io::pollout);
			}
			descriptor->context[0].ops.add_tail(op);
			if (ops) {
				if (!engine.multiplexer().bind(descriptor, ops)) {
					engine.post(mux_type::drain_ops(descriptor), cx:system_error());
				}
			}
		}

		template <typename HandlerType>
		static void recv(engine_type& engine, 
			const descriptor_type& descriptor,
			buffer_type& buf,
			HandlerType&& handler)
		{
			write_operator* op = new handler_operation<write_operator, HandlerType>(
				std::forward<HandlerType>(handler));
			
			if (!mux_type::good(descriptor)) {
				engine.post(mux_type::drain_ops(descriptor),
					std::make_error_code(std::errc::bad_file_descriptor));
				op->error(std::make_error_code(std::errc::invalid_argument));
				engine.post(op);
				return;
			}
			int ops = 0;
			if (descriptor->context[1].ops.empty()) {
				ops = cx::io::pollout | (descriptor->context[0].ops.empty() ? 0 : cx::io::pollin);
			}
			descriptor->context[1].ops.add_tail(op);
			if (ops) {
				if (!engine.multiplexer().bind(descriptor, ops)) {
					engine.post(mux_type::drain_ops(descriptor), cx:system_error());
				}
			}
		}
    };
}

#endif