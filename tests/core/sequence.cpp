/**
 * @brief 
 * 
 * @file sequence.cpp
 * @author ghtak
 * @date 2018-05-15
 */
#include <gtest/gtest.h>
#include <cx/core/mp.hpp>

TEST( cx_core , sequence ){
    static_assert( std::is_same< 
        cx::mp::make_sequence<4> ,
        cx::mp::sequence<  0 , 1 , 2 , 3 > >::value );

    static_assert( std::is_same< 
        cx::mp::make_sequence<1> ,
        cx::mp::sequence< 0 > >::value );
}