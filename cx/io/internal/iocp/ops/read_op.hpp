/**
 * */
#ifndef __cx_io_internal_iocp_ip_read_op_h__
#define __cx_io_internal_iocp_ip_read_op_h__

#include <cx/io/basic_read_op.hpp>
#include <cx/io/handler_op.hpp>

namespace cx::io::internal::iocp::ip {

    template < typename ServiceType , typename HandlerType >
    using read_op = cx::io::handler_op< basic_read_op<ServiceType>, HandlerType>;

}

#endif