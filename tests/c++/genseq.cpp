#include <gtest/gtest.h>
#include <cx/core/demangle.hpp>

// https://stackoverflow.com/questions/13072359/c11-compile-time-array-with-logarithmic-evaluation-depth

template<unsigned...> struct seq{ using type = seq; };

template<class S1, class S2> struct concat;

template<unsigned... I1, unsigned... I2>
struct concat<seq<I1...>, seq<I2...>>
    : seq<I1..., (sizeof...(I1)+I2)...>{};

template<unsigned N> struct gen_seq;
template<unsigned N>
struct gen_seq : concat<
            typename gen_seq<N/2>::type 
        ,   typename gen_seq<N - N/2>::type 
        >::type 
{

};

template<> struct gen_seq<0> : seq<>{};
template<> struct gen_seq<1> : seq<0>{};


TEST( gen_seq_test , usage ) {
    std::cout << cx::pretty_type_name( gen_seq<0>::type() ) << std::endl;
    std::cout << cx::pretty_type_name( gen_seq<1>::type() ) << std::endl;
/*
    struct gen_seq : concat<
        typename gen_seq<N/2>::type     // gen_seq<2/2>  => gen_seq<1>::type =>  seq<0>
    ,   typename gen_seq<N - N/2>::type // gen_seq<2 - 2/2> => gen_seq<1>::type =>  seq<0>
    >::type 
    {

    };
    template<unsigned... I1, unsigned... I2>
    struct concat<seq<I1...>, seq<I2...>>
    : seq< 0 , 1 + 0 >{};*/
    // gen_seq<2> - > seq< 0 , 1 >
    std::cout << cx::pretty_type_name( gen_seq<2>::type() ) << std::endl;
/*
    struct gen_seq : concat<
        typename gen_seq<N/2>::type     // gen_seq<1> seq<0>
    ,   typename gen_seq<N - N/2>::type // gen_seq<2>
    >::type 
    {

    };
    template<unsigned... I1, unsigned... I2>
    struct concat<seq<I1...>, seq<I2...>>
    : seq< 0 ,  1 + 0 , 1 + 1  >{}; //sizeof...(I1) == 1 */
    // seq< 0 , 1  , 2>
    std::cout << cx::pretty_type_name( gen_seq<3>::type() ) << std::endl;

    /*
    struct gen_seq : concat<
        typename gen_seq<N/2>::type     // gen_seq<4/2>   gen_seq<2> seq< 0 , 1 >
    ,   typename gen_seq<N - N/2>::type // gen_seq<4-4/2> gen_seq<2> seq< 0 , 1 >
    >::type 
    {

    };
    template<unsigned... I1, unsigned... I2>
    struct concat<seq<I1...>, seq<I2...>>
    : seq< 0 , 1 ,  2 + 0 , 2 + 1  >{}; ////sizeof...(I1) == 2
     seq< 0 , 1 , 2 , 3>
    */
    std::cout << "4" <<  cx::pretty_type_name( gen_seq<4>::type() ) << std::endl;
    std::cout << cx::pretty_type_name( gen_seq<5>::type() ) << std::endl;
    std::cout << cx::pretty_type_name( gen_seq<6>::type() ) << std::endl;
    std::cout << cx::pretty_type_name( gen_seq<10>::type()) << std::endl;

    std::cout << cx::pretty_type_name( concat<seq<> , seq<0>>::type()) << std::endl;
}

template < unsigned ... i_list >
struct eval {
    using type = eval;
};

template < unsigned ... i_list >
struct rebind : public eval < (sizeof(i_list)+i_list)... > {
};

TEST( tmp , rebind ) {
    std::cout << cx::pretty_type_name( rebind< 1 , 2 ,3 >::type{} ) << std::endl;
}

template < typename ... T >
struct _eval {
    typedef typename std::tuple_element< sizeof...(T) - 1 , std::tuple<T...> >::type type;
};

template < typename ... T >
struct dispose{
    using type = dispose;
};


// from crow
template <typename Seq, typename Tuple> 
struct pop_back_helper;

template <unsigned ... N, typename Tuple>
struct pop_back_helper<seq<N...>, Tuple>
{
    template <template <typename ... Args> class U>
    using rebind = U<typename std::tuple_element<N, Tuple>::type...>;
};

template <typename ... T>
struct pop_back
{
    template <template <typename ... Args> class U>
    using rebind = typename pop_back_helper< typename gen_seq<sizeof...(T)-1>::type  , std::tuple<T...>>::template rebind<U>;
};

// check
template < typename ... T >
struct type_list {
    using type = type_list;
};
template < typename Seq , typename Tuple >
struct helper;

template < unsigned ... N , typename Tuple >
struct helper< seq< N...>  , Tuple > {
    using type = type_list< typename std::tuple_element<N, Tuple>::type... >;
};

template < typename ... T >
struct pop_back0 {
    using type = typename helper< typename gen_seq<  sizeof...(T) - 1 >::type , std::tuple< T... > >::type;
};

template < typename ... T >
struct pop_back0 < type_list< T ... > >{
    using type = typename helper< typename gen_seq<  sizeof...(T) - 1 >::type , std::tuple< T... > >::type;
};

TEST( tmp , tuple_element ) {
    std::tuple< int , char , int , char > tup;
    std::cout << cx::pretty_type_name( tup ) << std::endl;

    _eval<  int , char , double , short >::type tup2;
    std::cout << cx::pretty_type_name( tup2 ) << std::endl;
    
    std::cout <<  cx::pretty_type_name(  pop_back<  int , char , void* >{}) << std::endl;
    std::cout <<  cx::pretty_type_name(  pop_back<  int , char , void* >::rebind<dispose>{} ) << std::endl;
    std::cout <<  cx::pretty_type_name(  pop_back<  int , char , void* >::rebind<dispose>::type{} ) << std::endl;
    std::cout <<  cx::pretty_type_name(  pop_back0< int , char , void* >::type{} ) << std::endl;
    std::cout <<  cx::pretty_type_name(  pop_back0< type_list< int , char , void* > >::type{} ) << std::endl;
}