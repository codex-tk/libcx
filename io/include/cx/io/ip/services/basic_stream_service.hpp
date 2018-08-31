/**
 * @brief 
 * 
 * @file basic_stream_service.hpp
 * @author ghtak
 * @date 2018-09-01
 */
#ifndef __cx_io_ip_basic_stream_service_h__
#define __cx_io_ip_basic_stream_service_h__
namespace cx::io::ip {

    template < typename MuxT > class basic_stream_service {
    public:
        using mux_t = MuxT;

    };

}
#endif