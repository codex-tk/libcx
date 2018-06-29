/**
 * */
#ifndef __cx_io_internal_iocp_ip_write_op_h__
#define __cx_io_internal_iocp_ip_write_op_h__

#include <cx/io/basic_write_op.hpp>
#include <cx/io/handler_op.hpp>

namespace cx::io::internal::iocp::ip {

    template < typename ServiceType , typename HandlerType >
    using write_op = cx::io::handler_op< basic_write_op<ServiceType>, HandlerType>;

}

#endif