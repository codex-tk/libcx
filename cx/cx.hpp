#ifndef __cx_h__
#define __cx_h__

#include <iostream>

namespace cx {
    std::string say_hello(void);
}

namespace {
  class defaulted_and_deleted_function_order_sample {
  public:
    // 소멸자 명시 선언
    // 이동 연산자는 delete 됨
    ~defaulted_and_deleted_function_order_sample(void) = default;

    // 이동 연산자 명시 선언
    // 이동 연산자 명시 선언시 복사 연산자는 delete 
    defaulted_and_deleted_function_order_sample(
      defaulted_and_deleted_function_order_sample&& 
    ) = default;
    defaulted_and_deleted_function_order_sample& operator=(
      defaulted_and_deleted_function_order_sample&& 
    ) = default;

    // 복사 연산자 명시 선언
    defaulted_and_deleted_function_order_sample(
      defaulted_and_deleted_function_order_sample&
    ) = default;
    defaulted_and_deleted_function_order_sample& operator=(
      defaulted_and_deleted_function_order_sample& 
    ) = default;
    
  };
}
#endif