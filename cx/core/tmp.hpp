/**
 * @brief template meta basic features
 * 
 * @file tmp.hpp
 * @author ghtak
 * @date 2018-05-12
 */
namespace cx::core {

    template < typename T , typename U >
    struct mem_ptr { typedef U T::* type; };

    template < typename T , typename S >
    struct mem_fun_ptr;

    template < typename T , typename R , typename ... Args >
    struct mem_fun_ptr< T , R ( Args ... ) >{
        typedef R (T::*type)( Args ... ) ;
    };

}