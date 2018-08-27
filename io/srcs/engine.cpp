/**
 * @brief
 *
 * @file engine.cpp
 * @author ghtak
 * @date 2018-08-27
 */
#include <cx/io/engine.hpp>

namespace cx::io {

	engine::engine(void)
		: _impl(*this) {
	}

	engine::~engine(void) {

	}

	cx::io::internal::impl_t& engine::implementation(void) {
		return _impl;
	}

}