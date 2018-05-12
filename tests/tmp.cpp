/**
 * @brief 
 * 
 * @file tmp.cpp
 * @author ghtak
 * @date 2018-05-12
 */
#include <gtest/gtest.h>
#include <cx/core/tmp.hpp>

namespace {
    class basic_object {
    public:
        int foo( void ){
            return _foo_val;
        }
        int bar( void ){
            return _var_val;
        }
    public:
        int _foo_val = 32;
        int _var_val = 81;
        int _member_variable;
    };
}

TEST( cx_tmp , mem_ptr ) {
    cx::core::mem_ptr< basic_object , int >::type mem_ptr = &basic_object::_member_variable;
    basic_object target_object;
    target_object._member_variable = 10;

    ASSERT_EQ( target_object.*mem_ptr , 10 );
    ASSERT_EQ( static_cast<basic_object*>(&target_object)->*mem_ptr , 10 );

    target_object.*mem_ptr = 11;
    ASSERT_EQ( target_object.*mem_ptr , 11 );
    ASSERT_EQ( static_cast<basic_object*>(&target_object)->*mem_ptr , 11 );
}

TEST( cx_tmp , mem_fun_ptr ) {
    cx::core::mem_fun_ptr< basic_object , int () >::type foo = &basic_object::foo;
    cx::core::mem_fun_ptr< basic_object , int () >::type bar = &basic_object::bar;
    basic_object target_object;
    ASSERT_EQ( (target_object.*foo)() , 31 );
    ASSERT_EQ( ((&target_object)->*bar)() , 81 );
}