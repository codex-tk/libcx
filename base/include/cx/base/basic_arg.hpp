/**
 * @brief
 *
 * @file basic_arg.hpp
 * @author ghtak
 * @date 2018-08-24
 */

#include <cx/base/defines.hpp>

namespace cx {

template <typename T> struct basic_arg {
    T value;
    basic_arg(T &&t) : value(std::forward<T>(t)) {}
};

template <typename T> struct basic_arg_generator {
    template <typename U> T operator=(U &&u) { return T(std::forward<U>(u)); }
    template <typename U> T operator()(U &&u) { return T(std::forward<U>(u)); }
};

using basic_arg_expend = int[];

} // namespace cx

#ifndef CX_DECLARE_ARG
#define CX_DECLARE_ARG(name, param_type)                                       \
    struct name {                                                              \
        param_type value;                                                      \
        name(param_type &&p) : value(std::forward<param_type>(p)) {}           \
    };                                                                         \
    using name##_generator_type = cx::basic_arg_generator<name>;               \
    static name##_generator_type name##_generator;
#endif

#ifndef CX_EXPAND_ARGS
#define CX_EXPAND_ARGS(set_func, ts_args)                                      \
    (void)cx::basic_arg_expend { 0, (set_func(ts_args), 0)... }
#endif