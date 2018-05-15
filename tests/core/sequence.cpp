/**
 * @brief 
 * 
 * @file sequence.cpp
 * @author ghtak
 * @date 2018-05-15
 */
#include <gtest/gtest.h>
#include <cx/core/sequence.hpp>

TEST( cx_core , sequence ){
    static_assert( std::is_same< 
        cx::core::make_sequence<4>::type ,
        cx::core::sequence<  0 , 1 , 2 , 3 > >::value );

    static_assert( std::is_same< 
        cx::core::make_sequence<1>::type ,
        cx::core::sequence< 0 > >::value );
}