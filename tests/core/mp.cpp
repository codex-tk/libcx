/**
 * @brief 
 * 
 * @file mp.cpp
 * @author ghtak
 * @date 2018-05-17
 */
#include <gtest/gtest.h>
#include <cx/core/tuple_sample.hpp>

TEST( cx_mp , tuple ) {
	cx::core::mp::internal::tuple_sample< int, double, char > t1{ 1 , 0.1 , 'c' };
    int value = cx::core::mp::internal::get<0>(t1);
    auto c_0 = std::integral_constant<std::size_t,0>();
    auto c_1 = std::integral_constant<std::size_t,1>(); 
    int value1 = t1[ c_0 ];
    double value2 = t1[ c_1 ];
    ASSERT_EQ( value , 1 );
    ASSERT_EQ( value , value1 );
	ASSERT_EQ( value2 , 0.1 );
	
    return;
}