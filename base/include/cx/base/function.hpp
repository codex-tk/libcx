/**
 * @brief
 *
 * @file function.hpp
 * @author ghtak
 * @date 2018-08-26
 */
#ifndef __cx_base_function_h__
#define __cx_base_function_h__

#include <cx/base/defines.hpp>
#include <memory>

namespace cx {
/*!
 */
template <typename Signature> class callable;

/*!
 */
template <typename R, typename... Args> class callable<R(Args...)> {
public:
    callable(void) noexcept {}
    virtual ~callable(void) noexcept {}
    virtual R operator()(Args &&... arg) = 0;
};

namespace {
template <typename HandlerT, typename R, typename... Args>
class callable_impl : public callable<R(Args...)> {
public:
    callable_impl(HandlerT &&h) noexcept
        : _handler(std::forward<HandlerT>(h)) {}
    virtual ~callable_impl(void) noexcept override {}
    virtual R operator()(Args &&... args) override {
        return _handler(std::forward<Args>(args)...);
    }

private:
    HandlerT _handler;
};
} // namespace

template <typename Signature> class function;

template <typename R, typename... Args> class function<R(Args...)> {
public:
    template <typename T>
    using callable_impl = cx::callable_impl<T, R, Args...>;

    /*! default construct */
    function(void) noexcept;
    /*! copy construct */
    function(const function &rhs);

    /*! move construct */
    function(function &&rhs) noexcept;

    /*! ctor from callable */
    template <typename HandlerT,
              typename = typename std::enable_if<!std::is_same_v<
                  function, std::remove_reference_t<HandlerT>>>::type>
    function(HandlerT &&handler);

    /*! typically assign operation */
    function &operator=(const function &rhs);

    /*! move assign */
    function &operator=(function &&rhs) noexcept;

    /*! assign from callable */
    template <typename HandlerT,
              typename = typename std::enable_if<!std::is_same_v<
                  function, std::remove_reference_t<HandlerT>>>::type>
    function &operator=(HandlerT &&handler);

    /*! */
    R operator()(Args &&... args) const;

    /*! */
    void swap(function &rhs);

private:
    std::shared_ptr<cx::callable<R(Args...)>> _callable;
};

template <typename R, typename... Args>
function<R(Args...)>::function(void) noexcept {}

template <typename R, typename... Args>
function<R(Args...)>::function(const function &rhs)
    : _callable(rhs._callable) {}

template <typename R, typename... Args>
function<R(Args...)>::function(function &&rhs) noexcept
    : _callable(std::move(rhs._callable)) {}

template <typename R, typename... Args>
template <typename HandlerT, typename>
function<R(Args...)>::function(HandlerT &&handler)
    : _callable(std::make_shared<callable_impl<HandlerT>>(
          std::forward<HandlerT>(handler))) {}
/*
        : _callable(std::allocate_shared< callable_impl<HandlerT> >(
                cx::basic_allocator<callable_impl<HandlerT>>(),
                std::forward< HandlerT>(handler))) {}
*/

template <typename R, typename... Args>
function<R(Args...)> &function<R(Args...)>::operator=(const function &rhs) {
    _callable = rhs._callable;
    return *this;
}

template <typename R, typename... Args>
function<R(Args...)> &function<R(Args...)>::operator=(function &&rhs) noexcept {
    _callable = std::move(rhs._callable);
    return *this;
}

template <typename R, typename... Args>
template <typename HandlerT, typename>
function<R(Args...)> &function<R(Args...)>::operator=(HandlerT &&handler) {
    _callable = std::make_shared<callable_impl<HandlerT>>(
        std::forward<HandlerT>(handler));
    return *this;
}

template <typename R, typename... Args>
R function<R(Args...)>::operator()(Args &&... args) const {
    if (_callable) {
        return (*_callable)(std::forward<Args>(args)...);
    }
    return R();
}

template <typename R, typename... Args>
void function<R(Args...)>::swap(function &rhs) {
    _callable.swap(rhs._callable);
}
} // namespace cx

#endif