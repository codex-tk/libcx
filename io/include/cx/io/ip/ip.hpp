/**
 * @brief 
 * 
 * @file ip.hpp
 * @author ghtak
 * @date 2018-09-09
 */
#ifndef __cx_io_ip_h__
#define __cx_io_ip_h__

#include <cx/base/defines.hpp>

namespace cx::io::ip{

    /**
     * @brief 
     * 
     * @tparam ServiceType 
     */
	template < typename ServiceType > struct is_dgram_available : std::false_type {};

    /**
     * @brief 
     * 
     * @tparam ServiceType 
     */
	template < typename ServiceType > struct is_stream_available : std::false_type {};
    
}

#endif