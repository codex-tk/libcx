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

TEST(cx_base_mp, sequence) {
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
	static_assert(std::is_same<gen_seq<0>::type, seq<>>::value);
	static_assert(std::is_same<gen_seq<1>::type, seq<0>>::value);
	static_assert(std::is_same<gen_seq<2>::type, seq<0, 1>>::value);
	static_assert(std::is_same<gen_seq<3>::type, seq<0, 1, 2>>::value);
	static_assert(std::is_same<gen_seq<4>::type, seq<0, 1, 2, 3>>::value);
	static_assert(std::is_same<concat<seq<>, seq<0>>::type, seq<0>>::value);
}

template < unsigned ... i_list >
struct eval {
	using type = eval;
};

template < unsigned ... i_list >
struct rebind : public eval < (sizeof(i_list) + i_list)... > {
};

TEST(cx_base_mp, rebind) {
	static_assert(std::is_same<rebind< 1, 2, 3 >::type, eval<5, 6, 7>>::value);
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
	static_assert(std::is_same<_eval<  int, char, double, short >::type, short>::value);
	static_assert(std::is_same<pop_back<  int, char, void* > , pop_back<int,char,void *>>::value);
	static_assert(std::is_same<pop_back<  int, char, void* >::rebind<dispose> , dispose<int,char>>::value);
	static_assert(std::is_same<pop_back<  int, char, void* >::rebind<dispose>::type, dispose<int,char>>::value);
	static_assert(std::is_same<pop_back0< int, char, void* >::type, type_list<int,char>>::value);
	static_assert(std::is_same<pop_back0< type_list< int, char, void* > >::type, type_list<int,char>>::value);
}


// ParameterPack

TEST(cpp_parameter_pack, comma_op) {
	int i, j;
	i = 0;
	j = i + 2;

	ASSERT_EQ(i, 0);
	ASSERT_EQ(j, 2);

	i = 10; j = 10;
	j = (i = 0, i + 2);

	ASSERT_EQ(i, 0);
	ASSERT_EQ(j, 2);
}

template < typename ... Ts >
std::string print_sample(Ts ... args) {
	//int arr [ sizeof...(args) + 2 ] = { 0 , args... , 4 };
	std::stringstream ss;
	using expend = int[];
	(void)expend {
		0, (ss << args, 0)...
	};
	return ss.str();
	//int dummy[sizeof...(Ts)] = { (std::cout << args, 0)... };
}

TEST(cpp_parameter_pack, print_sample) {
	ASSERT_EQ(print_sample(1, 2, 3, std::string("Test")), "123Test");
}

template < typename T, typename U, typename ... Ts >
void tuple_test(T t, U u, Ts ... ts) {
	std::tuple< T, U, Ts...> tuple0{ t , u ,ts... };
	std::tuple< T, Ts..., U> tuple1{ t , ts... , u };
	std::tuple< Ts..., T, U> tuple2{ ts... , t , u };
}

TEST(cpp_parameter_pack, tuple_test) {
	tuple_test(1, 2, 3, "Test");
}


template < typename ... Ts >
class base0 {
public:
	base0(std::stringstream& ss) {
		ss << cx::type_name(*this) << "|";
	}
};

template < typename ... Ts >
class base1 {
public:
	base1(std::stringstream& ss) { ss << cx::type_name(*this) << "|"; }
	base1(const int i) { }
	void g() { }
};


template < typename ... Ts >
class derived : private base0< Ts ... >,
	private base1< Ts, Ts ... > ...
{
public:
	// visuatl studio not works
	//using base1<Ts, Ts...>::g...;
	derived(std::stringstream& ss)
		: base0< Ts ... >(ss), base1< Ts, Ts ... >(ss)...
	{
	}
};

TEST(cpp_parameter_pack, derived) {
	std::stringstream ss;
	derived< int, double, char > object(ss);

#if defined(CX_PLATFORM_WIN32)
	ASSERT_EQ(ss.str(), "class base0<int,double,char>|class base1<int,int,double,char>|class base1<double,int,double,char>|class base1<char,int,double,char>|");
#else
	ASSERT_EQ(ss.str(), "base0<int, double, char>|base1<int, int, double, char>|base1<double, int, double, char>|base1<char, int, double, char>|");
#endif	
}

/*// visuatl studio not works
struct b0 { void g(){  } };
struct b1 { void g(){  } };
struct b2 { void g(){  } };

template <typename... bases>
struct X : bases... {
using bases::g...;
};

TEST( cpp_parameter_pack , X ) {
X< b0 , b1 , b2 > x;
}*/

#ifdef __GNUG__
namespace {
	template < typename ... Ts >
	int f(Ts ... ts) {
		return (... + ts); /// ((((( ts1 + ts2 ) + ts3) + ...  ) + ts(N-1) ) + ts(N))
						   // return ( ts + ... ); /// ts1 + ( ts2 + ( ts3 + ( ... + ( ts(N-1) + tsN ))))
	}

	template < typename ... Ts >
	int h(Ts ... ts) {
		return (... * ts);
	}

	template < typename ... Ts >
	int g(Ts ... ts) {
		return f(h(ts ...) + ts ...);
	}
}

TEST(cpp_parameter_pack, pack) {
	ASSERT_EQ(h(2, 3, 4), 2 * 3 * 4);
	ASSERT_EQ(f(2, 3, 4), 2 + 3 + 4);
	ASSERT_EQ(g(2, 3, 4), f(h(2, 3, 4) + 2, h(2, 3, 4) + 3, h(2, 3, 4) + 4));
}

#endif


template < typename R, typename ... Args >
class command {
public:
	command(void) {}
	~command(void) {}

	template < typename N, typename T > struct _impl {
		_impl(std::tuple< Args... >& tup, T& value) {
			std::get<N>() = value;
		}
	};

	template < typename T, typename ... > struct sp0;
	template <typename ... Ts> struct sp0< cx::mp::sequence<>, Ts ... > { sp0(std::tuple< Args... >& tup, ...) {} };
	template < std::size_t S0, std::size_t ... S, typename T, typename ... Ts >
	struct sp0< cx::mp::sequence<S0, S...>, T, Ts ... > : sp0< cx::mp::sequence< S...>, Ts ... > {
		sp0(std::tuple< Args... >& tup, T t, Ts ... ts)
			: sp0< cx::mp::sequence< S... >, Ts ... >(tup, ts...)
		{
			std::get<S0>(tup) = t;
		}
	};


	template < std::size_t N, typename T > struct tuple_setter {
		tuple_setter(std::tuple< Args... >& tup, T t) {
			std::get<N>(tup) = t;
		}
	};


	template < typename T, typename ... Ts > struct sp1;
	template < std::size_t ... S, typename ... Ts >
	struct sp1< cx::mp::sequence< S ... >, Ts ... > {
		sp1(std::tuple< Args... >& tup, Ts ... ts) {
			auto eval = { 0 , (tuple_setter<S,Ts>(tup,ts) , 0)... };
			(void)eval;
			//using expend = int[];
			//(void)expend{ 0 , (storage_helper<S,Ts>(tup,ts) , 0)... };
		}
	};

	template < std::size_t ... S >
	void set_params(cx::mp::sequence< S ... >, Args... args) {
		auto eval = { 0 , (tuple_setter<S,Args>(_params,args) , 0)... };
		(void)eval;
	}


	void set_params(Args ... args) {
		set_params(cx::mp::make_sequence<sizeof...(Args)>(), args ...);
		//sp1< typename cx::make_seq<sizeof...(Args)>::type , Args ... > s1( _params  , args... );
		//sp0< typename cx::make_seq<sizeof...(Args)>::type , Args ... > s0( _params  , args... );
	}

	void set_function(std::function< R(Args...) > func) {
		_func = func;
	}

	template < std::size_t ... S >
	R call(cx::mp::sequence< S... >) {
		return _func(std::get<S>(_params)...);
	}

	R operator()() {
		return call(cx::mp::make_sequence<sizeof...(Args)>());
	}

	std::tuple< Args ... >& params() {
		return _params;
	}
private:
	std::tuple< Args ... > _params;
	std::function< R(Args...) > _func;
};

TEST(tmp, delayed_command) {
	command<double, int, double, char > cmd;
	int p0 = 1;
	double p2 = 2.2;
	char p3 = 'c';
	cmd.set_params(p0, p2, p3);
	ASSERT_EQ(std::get<0>(cmd.params()), 1);
	ASSERT_EQ(std::get<1>(cmd.params()), 2.2);
	ASSERT_EQ(std::get<2>(cmd.params()), 'c');
	cmd.set_function([](int i, double v, char c) -> double {
		return i + v + c;
	});
	ASSERT_EQ(cmd(), 2.2 + 1 + 'c');
}
