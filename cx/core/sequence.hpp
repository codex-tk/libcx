/**
 * @brief 
 * 
 * @file sequence.hpp
 * @author ghtak
 * @date 2018-05-14
 */
namespace cx::core {

    template < std::size_t ... S > struct sequence{};
    
    /*
        make_sequence<4>
        make_sequence<3,3>
        make_sequence<2,2,3>
        make_sequence<1,1,2,3>
        make_sequence<0,0,1,2,3>::type -> sequence<0,1,2,3>
     */ 
    template < std::size_t I , std::size_t ... S > struct make_sequence
        : make_sequence< I - 1  ,  I - 1  , S ... >{};

    template < std::size_t ... S > struct make_sequence< 0 , S ... > {
        using type = sequence< S ... >;
    };
}