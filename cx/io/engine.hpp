/**
 * @brief
 *
 * @file engine.hpp
 * @author ghtak
 * @date 2018-06-03
 */
#ifndef __cx_io_engine_h__
#define __cx_io_engine_h__

#include <cx/io/async_layer.hpp>

namespace cx::io {

	class engine {
	public:
		engine(void) : _async_layer(*this) {}
		~engine(void) {}

		cx::io::async_layer& async_layer(void) {
			return _async_layer;
		}
	private:
		cx::io::async_layer _async_layer;
	};


}

#endif