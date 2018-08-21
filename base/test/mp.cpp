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
#include <cx/base/utils.hpp>

TEST(cx_base_mp , sequence) {
	static_assert(std::is_same<
		cx::mp::make_sequence<4>,
		cx::mp::sequence<0, 1, 2, 3>>::value);

	static_assert(std::is_same<
		cx::mp::make_sequence<1>,
		cx::mp::sequence<0>>::value);
}

TEST(cx_base_mp, type_list) {
	using tl = cx::type_list<int, double>;

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

	static_assert(std::is_same<cx::mp::transform<
		cx::type_list<int, double, char>, std::add_pointer>::type,
		cx::type_list<int *, double *, char *>>::value);
	static_assert(std::is_same<cx::mp::transform<
		std::tuple<int &, double, char>, std::remove_reference>::type,
		std::tuple<int, double, char>>::value);
}

TEST(cx_base_mp, tuple) {
	cx::mp::tuple< int, double, char > tuple{ 1 , 0.1 , 'c' };
	int value = cx::mp::get<0>(tuple);
	auto c_0 = std::integral_constant<std::size_t, 0>();
	auto c_1 = std::integral_constant<std::size_t, 1>();
	int value1 = tuple[c_0];
	double value2 = tuple[c_1];
	ASSERT_EQ(value, 1);
	ASSERT_EQ(value, value1);
	ASSERT_EQ(value2, 0.1);

}


// https://stackoverflow.com/questions/13072359/c11-compile-time-array-with-logarithmic-evaluation-depth

template<unsigned...> struct seq { using type = seq; };

template<class S1, class S2> struct concat;

template<unsigned... I1, unsigned... I2>
struct concat<seq<I1...>, seq<I2...>>
	: seq<I1..., (sizeof...(I1)+I2)...> {};

template<unsigned N> struct gen_seq;

template<unsigned N>
struct gen_seq : concat<
	typename gen_seq<N / 2>::type
	, typename gen_seq<N - N / 2>::type
>::type
{
};

template<> struct gen_seq<0> : seq<> {};
template<> struct gen_seq<1> : seq<0> {};

TEST(cx_base_mp, genseq) {
#if defined(CX_PLATFORM_WIN32)
	ASSERT_EQ(cx::type_name(gen_seq<0>::type()), "struct seq<>");
	ASSERT_EQ(cx::type_name(gen_seq<1>::type()), "struct seq<0>");
	ASSERT_EQ(cx::type_name(gen_seq<2>::type()), "struct seq<0,1>");
	ASSERT_EQ(cx::type_name(gen_seq<3>::type()), "struct seq<0,1,2>");
	ASSERT_EQ(cx::type_name(gen_seq<4>::type()), "struct seq<0,1,2,3>");
	ASSERT_EQ(cx::type_name(concat<seq<>, seq<0>>::type()), "struct seq<0>");
#else
	ASSERT_EQ(cx::type_name(gen_seq<0>::type()), "seq<>");
	ASSERT_EQ(cx::type_name(gen_seq<1>::type()), "seq<0u>");
	ASSERT_EQ(cx::type_name(gen_seq<2>::type()), "seq<0u, 1u>");
	ASSERT_EQ(cx::type_name(gen_seq<3>::type()), "seq<0u, 1u, 2u>");
	ASSERT_EQ(cx::type_name(gen_seq<4>::type()), "seq<0u, 1u, 2u, 3u>");
	ASSERT_EQ(cx::type_name(concat<seq<>, seq<0>>::type()), "seq<0u>");
#endif
}

template < unsigned ... i_list >
struct eval {
	using type = eval;
};

template < unsigned ... i_list >
struct rebind : public eval < (sizeof(i_list) + i_list)... > {
};

TEST(cx_base_mp, rebind) {
#if defined(CX_PLATFORM_WIN32)
	ASSERT_EQ(cx::type_name(rebind< 1, 2, 3 >::type{}), "struct eval<5,6,7>");
#else
	ASSERT_EQ(cx::type_name(rebind< 1, 2, 3 >::type{}), "eval<5u, 6u, 7u>");
#endif
}

template < typename ... T >
struct _eval {
	typedef typename std::tuple_element< sizeof...(T)-1, std::tuple<T...> >::type type;
};

template < typename ... T >
struct dispose {
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
	using rebind = typename pop_back_helper< typename gen_seq<sizeof...(T)-1>::type, std::tuple<T...>>::template rebind<U>;
};

// check
template < typename ... T >
struct type_list {
	using type = type_list;
};
template < typename Seq, typename Tuple >
struct helper;

template < unsigned ... N, typename Tuple >
struct helper< seq< N...>, Tuple > {
	using type = type_list< typename std::tuple_element<N, Tuple>::type... >;
};

template < typename ... T >
struct pop_back0 {
	using type = typename helper< typename gen_seq<  sizeof...(T)-1 >::type, std::tuple< T... > >::type;
};

template < typename ... T >
struct pop_back0 < type_list< T ... > > {
	using type = typename helper< typename gen_seq<  sizeof...(T)-1 >::type, std::tuple< T... > >::type;
};

TEST(cx_base_mp, tuple_element) {
	std::tuple< int, char, int, char > tup;
	_eval<  int, char, double, short >::type tup2;
#if defined(CX_PLATFORM_WIN32)
	ASSERT_EQ(cx::type_name(tup), "class std::tuple<int,char,int,char>");
	ASSERT_EQ(cx::type_name(tup2), "short");
	ASSERT_EQ(cx::type_name(pop_back<  int, char, void* >{}) , "struct pop_back<int,char,void *>");
	ASSERT_EQ(cx::type_name(pop_back<  int, char, void* >::rebind<dispose>{}) , "struct dispose<int,char>");
	ASSERT_EQ(cx::type_name(pop_back<  int, char, void* >::rebind<dispose>::type{}), "struct dispose<int,char>");
	ASSERT_EQ(cx::type_name(pop_back0< int, char, void* >::type{}), "struct type_list<int,char>");
	ASSERT_EQ(cx::type_name(pop_back0< type_list< int, char, void* > >::type{}), "struct type_list<int,char>");
	
#else
	ASSERT_EQ(cx::type_name(tup), "std::tuple<int, char, int, char>");
	ASSERT_EQ(cx::type_name(tup2), "short");
	ASSERT_EQ(cx::type_name(pop_back<  int, char, void* >{}), "pop_back<int, char, void*>");
	ASSERT_EQ(cx::type_name(pop_back<  int, char, void* >::rebind<dispose>{}), "dispose<int, char>");
	ASSERT_EQ(cx::type_name(pop_back<  int, char, void* >::rebind<dispose>::type{}), "dispose<int, char>");
	ASSERT_EQ(cx::type_name(pop_back0< int, char, void* >::type{}), "type_list<int, char>");
	ASSERT_EQ(cx::type_name(pop_back0< type_list< int, char, void* > >::type{}), "type_list<int, char>");
#endif	
}