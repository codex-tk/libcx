#include <gtest/gtest.h>
#include <cx/core/demangle.hpp>

#include <functional>
namespace {

template < unsigned ... > struct seq{};
template < unsigned max , unsigned ...s > struct make_seq0 { typedef int type; };
template < unsigned ... s > struct make_seq0< 0 , s ... > { typedef char type; };


template < unsigned max , unsigned ...s > struct make_seq1 { typedef seq<max - 1, max - 1, s... > type;  };
template < unsigned ... s > struct make_seq1< 0 , s ... > { typedef seq<s...> type;  };

}





TEST( make_seq , view ) {
    std::cout << cx::pretty_type_name( make_seq0<0>::type{} ) << std::endl; // char
    std::cout << cx::pretty_type_name( make_seq0<1>::type{} ) << std::endl; // int
    std::cout << cx::pretty_type_name( make_seq0<2>::type{} ) << std::endl; // int
    std::cout << cx::pretty_type_name( make_seq0<3>::type{} ) << std::endl; // int
    std::cout << cx::pretty_type_name( make_seq0<4>::type{} ) << std::endl; // int
    std::cout << cx::pretty_type_name( make_seq0<5>::type{} ) << std::endl; // int
}

TEST( make_seq , view2 ) {
    std::cout << cx::pretty_type_name( make_seq1<0>::type{} ) << std::endl; // char
    // make_seq1< 0 , s ... > - > make_seq1< 0 , none > -> seq<>

    std::cout << cx::pretty_type_name( make_seq1<1>::type{} ) << std::endl; // int
    // make_seq1< max = 1 , s ... = none  > ->  seq< 1 -1 , 1 -1 , none > -> seq< 1 -1 , 1 -1  > 

    std::cout << cx::pretty_type_name( make_seq1<2>::type{} ) << std::endl; // int
    std::cout << cx::pretty_type_name( make_seq1<3>::type{} ) << std::endl; // int
    std::cout << cx::pretty_type_name( make_seq1<4>::type{} ) << std::endl; // int
    std::cout << cx::pretty_type_name( make_seq1<5>::type{} ) << std::endl; // int
}

namespace sample {

	//https://stackoverflow.com/questions/17178499/how-does-stdget-work
	template < unsigned ... > struct seq {};
	//std::index_sequence 
	template < unsigned max, unsigned ...s > struct make_seq : make_seq< max - 1, max - 1, s ... > {};
	template < unsigned ... s > struct make_seq< 0, s ... > { using type = seq<s...>; };

}

TEST( make_seq , view3 ) {
    std::cout << cx::pretty_type_name( sample::make_seq<0>::type{} ) << std::endl; // char
    // make_seq< 0 , s ... > - > make_seq1< 0 , none > -> seq<>

    std::cout << cx::pretty_type_name( sample::make_seq<1>::type{} ) << std::endl; // int
    // make_seq< max = 1 , s ... = none  > ->  make_seq< 0 , 0 , none > -> seq<0>

    std::cout << cx::pretty_type_name( sample::make_seq<2>::type{} ) << std::endl; // int
    // make_seq< max = 2 , s ... = none  > -> : make_seq< 1 , 1 > -> : make_seq< 0 , 1  > -> seq< 0 , 1 >

    std::cout << cx::pretty_type_name( sample::make_seq<3>::type{} ) << std::endl; // int
    // make_seq< 3 , ... > - > make_seq< 2 , 2 , ... > -> make_seq< 1 , 1 , 2 , ... > 
    //-> make_seq< 0 , 0 , 1, 2 , ... > -> seq< 0 , 1 , 2 >
    
    std::cout << cx::pretty_type_name( sample::make_seq<4>::type{} ) << std::endl; // int
    std::cout << cx::pretty_type_name( sample::make_seq<5>::type{} ) << std::endl; // int

    std::cout << cx::pretty_type_name( sample::make_seq<6>::type{} ) << std::endl;
}
/*
TEST( tmp , type_list ) {
    sample::sample_tuple< int , double , char > tuple;
    sample::sample_tuple_get< 0 > (tuple) = 1;
    sample::sample_tuple_get< 1 > (tuple) = 1.1;
    sample::sample_tuple_get< 2 > (tuple) = 'c';

    ASSERT_EQ( sample::sample_tuple_get< 0 > (tuple) , 1  );
    ASSERT_EQ( sample::sample_tuple_get< 1 > (tuple) , 1.1  );
    ASSERT_EQ( sample::sample_tuple_get< 2 > (tuple) , 'c'  );

    std::cout << cx::pretty_type_name( 
        sample::type_list< sample::sample_tuple< int , double , char >>::type<0> {}  ) << std::endl;

    static_assert(std::is_same< sample::type_list< sample::sample_tuple< int , double , char >>::type<0> , int >::value);
    
    std::cout << cx::pretty_type_name( 
        sample::type_list< sample::sample_tuple< int , double , char , std::string >>::last {}  ) << std::endl;
    std::cout << cx::pretty_type_name( 
        sample::type_list< int , double , char >::type<0> {}  ) << std::endl;
    std::cout << cx::pretty_type_name( 
        sample::type_list< int , double , char >::last {}  ) << std::endl;
    std::cout << cx::pretty_type_name( 
        sample::type_list< std::tuple< int , double , char >>::type<0> {}  ) << std::endl;
    std::cout << cx::pretty_type_name( 
        sample::type_list< std::tuple< int , double , char >>::last {}  ) << std::endl;

    std::cout << cx::pretty_type_name( 
        sample::type_list< int ,char >::
                template push_back< double >::
                    template rebind< std::tuple >{} 
    ) << std::endl;

    std::cout << cx::pretty_type_name( 
        sample::type_list< std::tuple< int ,char > >::
                template push_back< double >::
                    template rebind< std::tuple >{} 
    ) << std::endl;

    std::cout << cx::pretty_type_name( 
        sample::type_list< std::tuple< int ,char > >::pop_back::
                    template rebind< std::tuple >{} 
    ) << std::endl;
}
*/
template < typename R , typename ... Args >
class command {
public: 
    command( void ) {}
    ~command( void ) {}

    template < typename N , typename T > struct _impl {
        _impl( std::tuple< Args... >& tup , T& value ) {
            std::get<N>() = value;
        }
    };

    template < typename T , typename ... > struct sp0;
    template <typename ... Ts> struct sp0< sample::seq<> , Ts ... > { sp0( std::tuple< Args... >& tup , ...  ) {} };
    template < unsigned S0 , unsigned ... S , typename T , typename ... Ts > 
    struct sp0< sample::seq<S0 , S...> , T , Ts ... > : sp0< sample::seq< S...> , Ts ... > {
        sp0( std::tuple< Args... >& tup , T t , Ts ... ts  )
            : sp0< sample::seq< S... > , Ts ... >( tup , ts... )
        {
            std::get<S0>(tup) = t;
        }
    };


    template < unsigned N  , typename T > struct tuple_setter { 
        tuple_setter( std::tuple< Args... >& tup  , T t ) {
            std::get<N>(tup) = t;
        }
    };
    

    template < typename T , typename ... Ts > struct sp1;
    template < unsigned ... S , typename ... Ts > 
    struct sp1< sample::seq< S ... > , Ts ... >  {
        sp1( std::tuple< Args... >& tup , Ts ... ts ) {
            auto eval = {0 , (tuple_setter<S,Ts>(tup,ts) , 0)... };
            (void)eval;
            //using expend = int[];
            //(void)expend{ 0 , (storage_helper<S,Ts>(tup,ts) , 0)... };
        }
    };

    template < unsigned ... S >
    void set_params( sample::seq< S ... > , Args... args ) {
        auto eval = {0 , (tuple_setter<S,Args>(_params,args) , 0)... };
        (void)eval;
    }


    void set_params( Args ... args ) {
        set_params( typename sample::make_seq<sizeof...(Args)>::type{} , args ... );
        //sp1< typename cx::make_seq<sizeof...(Args)>::type , Args ... > s1( _params  , args... );
        //sp0< typename cx::make_seq<sizeof...(Args)>::type , Args ... > s0( _params  , args... );
    }

    void set_function(std::function< R (Args...) > func) {
        _func = func;
    }

    template < unsigned ... S >
    R call( sample::seq< S... > ) {
        return _func( std::get<S>(_params)... );
    }

    R operator()(){
        return call( typename sample::make_seq<sizeof...(Args)>::type{} );
    }

     std::tuple< Args ... >& params() {
         return _params;
     }
private:
    std::tuple< Args ... > _params;
    std::function< R (Args...) > _func;
};

TEST( tmp , delayed_command ) {
    command<double , int , double , char > cmd;
    int p0 = 1;
    double p2 = 2.2;
    char p3 = 'c';
    cmd.set_params( p0 , p2 , p3 );
    ASSERT_EQ( std::get<0>(cmd.params()) , 1 );
    ASSERT_EQ( std::get<1>(cmd.params()) , 2.2 );
    ASSERT_EQ( std::get<2>(cmd.params()) , 'c' );
    cmd.set_function( []( int i , double v , char c ) -> double {
        return i + v + c;
    });
    ASSERT_EQ( cmd() , 2.2 + 1 + 'c' );
}
