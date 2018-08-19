/**
 * @brief
 *
 * @file utils.hpp
 * @author ghtak
 * @date 2018-08-19
 */
#ifndef __cx_base_utils_h__
#define __cx_base_utils_h__

#include <cx/base/defines.hpp>

namespace cx::base {

	/**
	 * @brief
	 *
	 * @param p
	 * @param sz
	 * @return uint16_t
	 */
	uint16_t checksum(void* p, int sz);

	/**
	 * @brief
	 *
	 * @param name
	 * @return std::string
	 */
	std::string demangle(const char *name);

	template <typename T>
	std::string type_name(const T& t) {
		return demangle(typeid(t).name());
	}
}

#endif