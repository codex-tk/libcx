/**
 * @brief 
 * 
 * @file type_list.hpp
 * @author ghtak
 * @date 2018-05-12
 */

#ifndef __cx_core_type_list_h__
#define __cx_core_type_list_h__

#include <cx/core/mp.hpp>

namespace cx::core{
    
    template < typename ... Ts >
    struct type_list{  

        using self = type_list< Ts ... >;

        template < std::size_t I > 
        using at = cx::core::mp::at< I , self >;

        using front = cx::core::mp::front< self>;

        using back = cx::core::mp::back< self>;
        
        template < typename ... Us >
        using push_back = cx::core::mp::push_back< self , Us ... >;

        template < typename ... Us >
        using push_front = cx::core::mp::push_front< self , Us ... >;

        using pop_front = cx::core::mp::pop_front< self >;

        using pop_back  = cx::core::mp::pop_back< cx::core::mp::make_sequence<sizeof...(Ts) - 1> , self >;

        using size = cx::core::mp::size< Ts ... >;

        template < std::size_t I > 
        using at_t = typename cx::core::mp::at< I , type_list< Ts ... >>::type;
    
        template < template < typename ... > class U >
        struct rebind {
            using other = U< Ts ... >;
        };
    };
}
#endif