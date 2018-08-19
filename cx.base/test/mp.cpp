/**
 * @brief 
 * 
 * @file mp.cpp
 * @author ghtak
 * @date 2018-05-17
 */
#include <gtest/gtest.h>
#include <cx/base/mp.hpp>
#include <cx/base/type_list.hpp>
#include <cx/base/tuple.hpp>

TEST(cx_base, sequence) {
    static_assert(std::is_same<
                  cx::base::mp::make_sequence<4>,
                  cx::base::mp::sequence<0, 1, 2, 3>>::value);

    static_assert(std::is_same<
                  cx::base::mp::make_sequence<1>,
                  cx::base::mp::sequence<0>>::value);

    using tl = cx::base::type_list<int, double>;
	
	static_assert(std::is_same<tl::at<0>::type, int>::value);
    static_assert(std::is_same<tl::at<1>::type, double>::value);

    static_assert(std::is_same<tl::front::type, int>::value);
    static_assert(std::is_same<tl::back::type, double>::value);

    static_assert(std::is_same<tl::push_back<char>::type::at_t<2>, char>::value);
    static_assert(std::is_same<tl::push_front<char>::type::at_t<0>, char>::value);
    static_assert(std::is_same<tl::push_front<char>::type::at_t<1>, int>::value);
    static_assert(std::is_same<tl::push_front<char>::type::at_t<2>, double>::value);

    static_assert(std::is_same<tl::pop_front::type::at_t<0>, double>::value);

    static_assert(std::is_same<tl::pop_back::type::at_t<0>, int>::value);

    static_assert(std::is_same<tl::at_t<0>, int>::value);
    static_assert(std::is_same<tl::at_t<1>, double>::value);

    ASSERT_EQ(tl::pop_back::type::size::value, static_cast<std::size_t>(1));
    ASSERT_EQ(tl::size::value, static_cast<std::size_t>(2));

    tl::rebind<std::tuple>::other sample_tuple;

    static_assert(std::is_same<cx::base::mp::transform<
                                   cx::base::type_list<int, double, char>, std::add_pointer>::type,
                               cx::base::type_list<int *, double *, char *>>::value);
    static_assert(std::is_same<cx::base::mp::transform<
                                   std::tuple<int &, double, char>, std::remove_reference>::type,
                               std::tuple<int, double, char>>::value);


	cx::base::mp::tuple_sample< int, double, char > tuple{ 1 , 0.1 , 'c' };
	int value = cx::base::mp::get<0>(tuple);
	auto c_0 = std::integral_constant<std::size_t, 0>();
	auto c_1 = std::integral_constant<std::size_t, 1>();
	int value1 = tuple[c_0];
	double value2 = tuple[c_1];
	ASSERT_EQ(value, 1);
	ASSERT_EQ(value, value1);
	ASSERT_EQ(value2, 0.1);

}
/*

TEST( cx_mp , tuple ) {

    return;
}*/