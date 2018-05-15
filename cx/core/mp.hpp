/**
 * @brief 
 * 
 * @file mp.hpp
 * @author ghtak
 * @date 2018-05-13
 */
#ifndef __cx_core_mp_h__
#define __cx_core_mp_h__

#include <cx/core/sequence.hpp> 

namespace cx::core::mp {

#if defined( USE_CX_SEQ_IMPL )
    template < std::size_t ... S > using seq_impl = cx::core::sequence< S ... >;
    template < std::size_t S >  using make_seq_impl = typename cx::core::make_sequence< S >::type;
#else
    template < std::size_t ... S > using seq_impl = std::index_sequence< S ... >;
    template < std::size_t S >  using make_seq_impl = std::make_index_sequence< S >;
#endif

    template < std::size_t I , typename T > struct at;
    
    template < std::size_t I ,
        template < typename ... > class TypeList ,
        typename Head ,
        typename ... Ts >
    struct at< I , TypeList< Head , Ts ... > >
        : at< I - 1 , TypeList< Ts  ... > >  {
    };

    template < template < typename ... > class TypeList,
        typename Head ,
        typename ... Ts >
    struct at< 0 , TypeList< Head , Ts ... > > {
        using type = Head;
    };

    template < typename TypeList >
    using front = at< 0 , TypeList >;
        
    template < typename TypeList > struct back;

    template < template < typename ...> class TypeList , typename ... Ts >
    struct back< TypeList< Ts ... >> {
        using type = typename at< sizeof...(Ts) - 1 , TypeList< Ts ... >>::type;
    };

    template < typename TypeList , typename U > struct push_back;

    template < template < typename ... > class TypeList 
            , typename ... Us 
            , typename ... Ts >
    struct push_back< TypeList< Ts ... > , Us ...  >{
        using type = TypeList< Ts ... , Us ... >;
    };
    
    template < typename TypeList , typename U > struct push_front;

    template < template < typename ... > class TypeList 
            , typename ... Us 
            , typename ... Ts >
    struct push_front< TypeList< Ts ... > , Us ... >{
        using type = TypeList< Us ... , Ts ... >;
    };

    template < typename TypeList > struct pop_front;

    template < template < typename ... > class TypeList 
            , typename Head 
            , typename ... Ts >
    struct pop_front< TypeList< Head , Ts ... > > {
        using type = TypeList< Ts ... >;
    };

    template < typename I , typename TypeList > struct pop_back;

    template < std::size_t ... I 
            , template < typename ... > class TypeList
            , typename ... Ts >
    struct pop_back< cx::core::mp::seq_impl< I ... > , TypeList< Ts ... > >{
        using type = TypeList< typename TypeList<Ts ...>::template at<I>::type ... >;
    };

    template < typename TypeList > struct size;

    template < template < typename ... > class TypeList
             , typename ... Ts >
    struct size< TypeList< Ts ... >> {
        static constexpr std::size_t value = 
                std::integral_constant<std::size_t, sizeof...(Ts)>::value;
    };
}

#endif