/**
 * @file rio.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2018-10-14
 *
 * @copyright Copyright (c) 2018
 *
 */

#ifndef __cx_io_mux_rio_h__
#define __cx_io_mux_rio_h__

#include <cx/base/defines.hpp>
#include <cx/base/noncopyable.hpp>
#include <cx/io/basic_operation.hpp>
#include <cx/io/io.hpp>

#if defined(CX_PLATFORM_WIN32) && defined(CX_EXPERIMENT_RIO)

namespace cx::io {
	template < typename MuxT >
	class basic_engine;
}

namespace cx::io::mux {

class registered_io : private cx::noncopyable {
public:
    using this_type = registered_io;

    struct descriptor;

    using descriptor_type = std::shared_ptr<descriptor>;
    using operation_type = basic_operation<descriptor_type>;

    struct RIO_BUF_EX : RIO_BUF {
        descriptor_type descriptor;
        cx::io::type type;

        void hold(const descriptor_type& fd) { descriptor = fd; }
		void unhold(void) { descriptor = nullptr; }
    };

    using socket_type = SOCKET;

    struct descriptor : private cx::noncopyable,
                        public std::enable_shared_from_this<descriptor> {
        union {
            socket_type s;
            HANDLE h;
        } fd;
        basic_engine<this_type> &engine;
    };
private:

};

} // namespace cx::io::mux

#endif
#endif