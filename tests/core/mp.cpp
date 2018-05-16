/**
 * @brief 
 * 
 * @file mp.cpp
 * @author ghtak
 * @date 2018-05-17
 */
#include <gtest/gtest.h>
#include <cx/core/tuple_sample.hpp>

template < typename ... Ts >
using tuple = cx::core::mp::detail::values0< 
            cx::core::mp::make_sequence< sizeof...(Ts) > 
            , cx::core::type_list< Ts ... > >;

TEST( cx_mp , tuple ) {
	tuple< int, double, char > t1{ 1 , 0.1 , 'c' };
	return;
}