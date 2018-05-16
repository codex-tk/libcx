/**
 * @brief 
 * 
 * @file tuple_sample.hpp
 * @author ghtak
 * @date 2018-05-17
 */
#ifndef __cx_core_mp_tuple_sample_h__
#define __cx_core_mp_tuple_sample_h__

#include <cx/core/type_list.hpp>

namespace cx::core::mp::detail{
    template < std::size_t N , typename T > struct value { 
        T data; 
        value(void) {}
        value(T&& t) : data(t) {}
    };
    
    template < typename I , typename ... Ts > struct values;
    
    template < std::size_t ... Is , typename ... Ts > 
    struct values< cx::core::mp::sequence< Is ... > , Ts ...  >
        : value< Is , Ts > ... 
    {
    
    };

    template < typename I , typename T > struct values0;

    template < std::size_t ... Is , typename ... Ts > 
    struct values0< cx::core::mp::sequence< Is ... > , type_list< Ts ... > >
        : value< Is , typename cx::core::mp::at< Is , type_list< Ts ... >>::type > ... 
    {
        values0( Ts&& ...  args )
            : value< Is , typename cx::core::mp::at< Is, type_list< Ts ... >>::type >( std::forward<Ts>(args) ) ... 
        {}
    };
}

#endif