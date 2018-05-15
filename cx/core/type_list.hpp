/**
 * @brief 
 * 
 * @file type_list.hpp
 * @author ghtak
 * @date 2018-05-12
 */

#include <cx/core/sequence.hpp>
namespace cx::core{
    
    template < typename ... Ts > struct type_list;
#if defined( USE_CX_SEQ_IMPL )
    template < std::size_t ... S > using seq_impl = cx::core::sequence< S ... >;
    template < std::size_t S >  using make_seq_impl = typename cx::core::make_sequence< S >::type;
#else
    template < std::size_t ... S > using seq_impl = std::index_sequence< S ... >;
    template < std::size_t S >  using make_seq_impl = std::make_index_sequence< S >;
#endif
    namespace impl {
        /**
         * @brief find the type in type_list by index
         * 
         * @tparam  
         * @tparam T 
         */
        template < std::size_t I , typename T > struct at;

        template < std::size_t I , typename Head , typename ... Ts  > 
        struct at< I , type_list< Head , Ts ... > > 
                : at< I - 1 , type_list< Ts ... > >{
        };

        template < typename Head , typename ... Ts >
        struct at< 0 , type_list< Head , Ts ...> > {
            using type = Head;
        };

        template < typename Head , typename ... Ts >
        struct pop_front {
            using type = type_list< Ts ... >;
        };

        template < typename I , typename T > struct pop_back;
        template < std::size_t ... I , typename ... Ts > 
        struct pop_back< seq_impl< I ... > , type_list< Ts ... > >
        { 
            using type = type_list< typename type_list< Ts ... >::template at<I>::type ... >; 
        };
    }

    template < typename ... Ts >
    struct type_list{  
        template < std::size_t I > 
        using at = impl::at< I , type_list< Ts ... > >;

        using front = at<0>;
        using back = at< sizeof ... ( Ts ) - 1 >;
    
        template < typename U >
        struct push_back {
            using type = type_list< Ts ... , U >;
        };

        template < typename U >
        struct push_front {
            using type = type_list< U , Ts ...  >;
        };

        using pop_front = impl::pop_front< Ts ... >;

        using pop_back  = impl::pop_back< make_seq_impl<sizeof...(Ts) - 1> 
                , type_list< Ts... > >;

        enum { size = sizeof...(Ts)  };

        template < std::size_t I > 
        using at_t = typename impl::at< I , type_list< Ts ... > >::type;
    
        template < template < typename ... Us > class U >
        struct rebind {
            using other = U< Ts ... >;
        };
    };
}