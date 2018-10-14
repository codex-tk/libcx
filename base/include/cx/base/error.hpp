/**
 * @brief
 *
 * @file error.hpp
 * @author ghtak
 * @date 2018-08-21
 */
#ifndef __cx_base_error_h__
#define __cx_base_error_h__

#include <cx/base/defines.hpp>
#include <map>

namespace cx {
/**
 * @brief
 *
 */
enum class errc {
    success = 0,
    operation_canceled = 1,
};

/**
 * @brief DL Lookup 으로 std 에서 해당 함수를 찾아서 사용
 *  cx::errc 와 동일한 네임스페이스에 존재해야한다.
 *  template <> struct is_error_code_enum<cx::errc> : true_type {};
 *
 * @param ec
 * @return std::error_code
 */
std::error_code make_error_code(cx::errc ec);

/**
 * @brief DL Lookup 으로 std 에서 해당 함수를 찾아서 사용
 *  cx::errc 와 동일한 네임스페이스에 존재해야한다.
 *  template <> struct is_error_code_enum<cx::errc> : true_type {};
 *
 * @param ec
 * @return std::error_code
 */
std::error_condition make_error_condition(cx::errc ec);

std::error_code system_error(void);
} // namespace cx

namespace std {
// template <> struct is_error_condition_enum<cx::errc> : true_type {};
template <> struct is_error_code_enum<cx::errc> : true_type {};
} // namespace std

#endif