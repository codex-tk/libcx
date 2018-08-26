#include <gtest/gtest.h>
#include <cx/base/function.hpp>

TEST( cx_base_function  , owner_ship ) {
    int seed = 0;
    cx::function< int (void) > origin( [&seed] () -> int {
        ++seed;
        return seed;
    });

    origin();
    ASSERT_EQ( seed , 1 );

    cx::function< int (void) > copy( origin );
    copy();
    ASSERT_EQ( seed , 2 );

    cx::function< int (void) > move( std::move( copy));
    copy();
    ASSERT_EQ( seed , 2 );
    move();
    ASSERT_EQ( seed , 3 );

    cx::function< int (void) > assign;
    assign = origin;
    assign();
    ASSERT_EQ( seed , 4 );

    assign = std::move( origin );
    assign();
    ASSERT_EQ( seed , 5 );

    origin();
    ASSERT_EQ( seed , 5 );
}