#include <gtest/gtest.h>
#include <cx/core/function.hpp>

#if defined ( CX_FUNCTION_TEST )
class function_test_fixture : public testing::Test {
public:
    void SetUp() override {
        method_calls = &cx::core::test::method_calls< cx::core::function < void () >>::instance();
        method_calls->reset();
        call_ids = {
            cx::core::test::function_ctor , 
            cx::core::test::function_copy_ctor ,
            cx::core::test::function_copy_ctor_l ,
            cx::core::test::function_move_ctor ,
            cx::core::test::function_callable_ctor ,
            cx::core::test::function_assign ,
            cx::core::test::function_assign_l ,
            cx::core::test::function_move_assign ,
            cx::core::test::function_callable_assign ,
        };
    }

    void TearDown() override {

    }
protected:
    cx::core::test::method_calls<cx::core::function < void () >>* method_calls;
    std::vector< int > call_ids;
};

typedef cx::core::function< void () > test_fn;

TEST_F( function_test_fixture , ctor ) {
    for ( auto id : call_ids ) {
        ASSERT_EQ( method_calls->call_counts(id) , 0 );
    }
    test_fn _function_ctor;
    ASSERT_EQ( method_calls->call_counts(cx::core::test::function_ctor) , 1 );

    const test_fn _function_copy_ctor_l(_function_ctor);
    ASSERT_EQ( method_calls->call_counts(cx::core::test::function_copy_ctor_l) , 1 );

    test_fn _function_copy_ctor( _function_copy_ctor_l );
    ASSERT_EQ( method_calls->call_counts(cx::core::test::function_copy_ctor) , 1 );

    test_fn _function_move_ctor( std::move(_function_copy_ctor));
    ASSERT_EQ( method_calls->call_counts(cx::core::test::function_move_ctor) , 1 );

    test_fn _function_callable_ctor([]{});
    ASSERT_EQ( method_calls->call_counts(cx::core::test::function_callable_ctor) , 1 );
}

TEST_F( function_test_fixture , assign ) {
    for ( auto id : call_ids ) {
        ASSERT_EQ( method_calls->call_counts(id) , 0 );
    }
    test_fn _function_ctor_rhs;
    const test_fn _function_ctor_const_rhs;
    ASSERT_EQ( method_calls->call_counts(cx::core::test::function_ctor) , 2 );

    test_fn _function_assign;
    ASSERT_EQ( method_calls->call_counts(cx::core::test::function_ctor) , 3 );

    _function_assign = _function_ctor_const_rhs;
    ASSERT_EQ( method_calls->call_counts(cx::core::test::function_assign) , 1 );
    
    _function_assign = _function_ctor_rhs;
    ASSERT_EQ( method_calls->call_counts(cx::core::test::function_assign_l) , 1 );

    _function_assign = std::move(_function_ctor_rhs);
    ASSERT_EQ( method_calls->call_counts(cx::core::test::function_move_assign) , 1 );

    _function_assign = [] {};
    ASSERT_EQ( method_calls->call_counts(cx::core::test::function_callable_assign) , 1 );
}
#endif

TEST( function  , owner_ship ) {
    int seed = 0;
    cx::core::function< int (void) > origin( [&seed] () -> int {
        ++seed;
        return seed;
    });

    origin();
    ASSERT_EQ( seed , 1 );

    cx::core::function< int (void) > copy( origin );
    copy();
    ASSERT_EQ( seed , 2 );

    cx::core::function< int (void) > move( std::move( copy));
    copy();
    ASSERT_EQ( seed , 2 );
    move();
    ASSERT_EQ( seed , 3 );

    cx::core::function< int (void) > assign;
    assign = origin;
    assign();
    ASSERT_EQ( seed , 4 );

    assign = std::move( origin );
    assign();
    ASSERT_EQ( seed , 5 );

    origin();
    ASSERT_EQ( seed , 5 );
}