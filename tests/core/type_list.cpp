/**
 * @brief 
 * 
 * @file type_list.cpp
 * @author ghtak
 * @date 2018-05-12
 */

#include <gtest/gtest.h>
#include <cx/core/type_list.hpp>

TEST( cx_core , type_list ){
    using tl = cx::core::type_list< int , double >;
    static_assert(  std::is_same< tl::at<0>::type , int >::value );
    static_assert(  std::is_same< tl::at<1>::type , double >::value );

    static_assert(  std::is_same< tl::front::type , int >::value );
    static_assert(  std::is_same< tl::back::type , double >::value );
    
    static_assert(  std::is_same< tl::push_back< char >::type::at_t<2> , char >::value);
    
    static_assert(  std::is_same< tl::push_front< char >::type::at_t<0> , char >::value);
    static_assert(  std::is_same< tl::push_front< char >::type::at_t<1> , int >::value);
    static_assert(  std::is_same< tl::push_front< char >::type::at_t<2> , double >::value);
    
    static_assert(  std::is_same< tl::pop_front::type::at_t<0> , double >::value);

    static_assert(  std::is_same< tl::pop_back::type::at_t<0> , int >::value);
    
    static_assert(  std::is_same< tl::at_t<0> , int >::value );
    static_assert(  std::is_same< tl::at_t<1> , double >::value );


    ASSERT_EQ( tl::pop_back::type::size::value  , static_cast<std::size_t>(1) );
    ASSERT_EQ( tl::size::value  , static_cast<std::size_t>(2) );

    tl::rebind< std::tuple >::other sample_tuple;

    static_assert( std::is_same< cx::core::mp::transform< 
        cx::core::type_list< int , double , char >
        , std::add_pointer >::type , cx::core::type_list< int* , double* , char* > >::value );
    static_assert( std::is_same< cx::core::mp::transform< 
        std::tuple< int& , double , char >
        , std::remove_reference >::type , std::tuple< int , double , char > >::value );

}
