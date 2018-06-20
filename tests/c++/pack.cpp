#include <gtest/gtest.h>
#include <cx/core/demangle.hpp>

TEST( pack , comma_op ) {
    int i , j;
    i = 0;
    j = i + 2;

    ASSERT_EQ( i , 0 );
    ASSERT_EQ( j , 2 );

    i = 10; j = 10;
    j = ( i = 0 , i+2);

    ASSERT_EQ( i , 0 );
    ASSERT_EQ( j , 2 );
}

template < typename ... Ts >
void print_sample( Ts ... args ) {
    //int arr [ sizeof...(args) + 2 ] = { 0 , args... , 4 };
    using expend = int[];
    (void)expend{ 0 , (std::cout << args, 0)... };
    //int dummy[sizeof...(Ts)] = { (std::cout << args, 0)... };
}

TEST( pack , print_sample ) {
    print_sample( 1 , 2, 3 , "Test" );
}

template < typename T , typename U , typename ... Ts >
void tuple_test( T t , U u , Ts ... ts ) {
    std::tuple< T , U , Ts...> tuple0{ t , u ,ts...};
    std::tuple< T , Ts... , U> tuple1{ t , ts... , u };
    std::tuple< Ts... , T , U> tuple2{ ts... , t , u };
}

TEST( pack , tuple_test ) {
    tuple_test( 1 , 2, 3 , "Test" );
}


template < typename ... Ts >
class base0{
public:
    base0() {
        std::cout << cx::pretty_type_name( *this ) << std::endl;
    }
};

template < typename ... Ts >
class base1{
public:
    base1() { std::cout << cx::pretty_type_name( *this ) << std::endl; }
    base1( const int i ) { }
    void g() { }
};


template < typename ... Ts >
class derived : private base0< Ts ... > ,
                private base1< Ts , Ts ... > ... 
{
public:
    // visuatl studio not works
    //using base1<Ts, Ts...>::g...;
    derived(){
    }
};

TEST( pack , derived ) {
    derived< int , double , char > object;
    /*
    1: class base0<int,double,char>
    1: class base1<int,int,double,char>
    1: class base1<double,int,double,char>
    1: class base1<char,int,double,char>
    */
}

/*// visuatl studio not works
struct b0 { void g(){  } };
struct b1 { void g(){  } };
struct b2 { void g(){  } };

template <typename... bases>
struct X : bases... {
	using bases::g...;
};

TEST( pack , X ) {
    X< b0 , b1 , b2 > x;
}*/

#ifdef __GNUG__
namespace {
    template < typename ... Ts >
    int f ( Ts ... ts ) {
        return ( ... + ts ); /// ((((( ts1 + ts2 ) + ts3) + ...  ) + ts(N-1) ) + ts(N))
        // return ( ts + ... ); /// ts1 + ( ts2 + ( ts3 + ( ... + ( ts(N-1) + tsN ))))
    }

    template < typename ... Ts >
    int h( Ts ... ts ) {
        return ( ... * ts );
    }

    template < typename ... Ts >
    int g( Ts ... ts ) {
        return f ( h( ts ... ) + ts ... );
    }
}

TEST( pack , pack ) {
    ASSERT_EQ( h( 2 , 3 , 4 ) , 2 * 3 * 4 );
    ASSERT_EQ( f( 2 , 3 , 4 ) , 2 + 3 + 4 );
    ASSERT_EQ( g(2,3,4) , f( h( 2 , 3 , 4 ) + 2 , h( 2 , 3 , 4 ) + 3 , h( 2 , 3 , 4 ) + 4 ) );
}

#endif