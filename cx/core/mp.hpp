/**
 * @brief
 *
 * @file mp.hpp
 * @author ghtak
 * @date 2018-05-13
 */
#ifndef __cx_core_mp_h__
#define __cx_core_mp_h__

#define USE_CX_SEQ_IMPL

namespace cx::mp {

#if defined( USE_CX_SEQ_IMPL )
	template < std::size_t ... S > struct sequence {};
	/*
		make_sequence<4>
		make_sequence<3,3>
		make_sequence<2,2,3>
		make_sequence<1,1,2,3>
		make_sequence<0,0,1,2,3>::type -> sequence<0,1,2,3>
	 */
	template < std::size_t I, std::size_t ... S > struct _make_sequence
		: _make_sequence< I - 1, I - 1, S ... > {};

	template < std::size_t ... S > struct _make_sequence< 0, S ... > {
		using type = sequence< S ... >;
	};
	template < std::size_t S > using make_sequence = typename _make_sequence< S >::type;
#else
	template < std::size_t ... S > using sequence = std::index_sequence< S ... >;
	template < std::size_t S >  using make_sequence = std::make_index_sequence< S >;
#endif

	template < std::size_t I, typename T > struct at;

	template < std::size_t I,
		template < typename ... > class TypeList,
		typename Head,
		typename ... Ts >
		struct at< I, TypeList< Head, Ts ... > >
		: at< I - 1, TypeList< Ts  ... > > {
	};

	template < template < typename ... > class TypeList,
		typename Head,
		typename ... Ts >
		struct at< 0, TypeList< Head, Ts ... > > {
		using type = Head;
	};

	template < typename TypeList >
	using front = at< 0, TypeList >;

	template < typename TypeList > struct back;

	template < template < typename ...> class TypeList, typename ... Ts >
	struct back< TypeList< Ts ... >> {
		using type = typename at< sizeof...(Ts)-1, TypeList< Ts ... >>::type;
	};

	template < typename TypeList, typename ... U > struct push_back;

	template < template < typename ... > class TypeList
		, typename ... Us
		, typename ... Ts >
		struct push_back< TypeList< Ts ... >, Us ...  > {
		using type = TypeList< Ts ..., Us ... >;
	};

	template < typename TypeList, typename ... U > struct push_front;

	template < template < typename ... > class TypeList
		, typename ... Us
		, typename ... Ts >
		struct push_front< TypeList< Ts ... >, Us ... > {
		using type = TypeList< Us ..., Ts ... >;
	};

	template < typename TypeList > struct pop_front;

	template < template < typename ... > class TypeList
		, typename Head
		, typename ... Ts >
		struct pop_front< TypeList< Head, Ts ... > > {
		using type = TypeList< Ts ... >;
	};

	template < typename I, typename TypeList > struct pop_back;

	template < std::size_t ... I
		, template < typename ... > class TypeList
		, typename ... Ts >
		struct pop_back< sequence< I ... >, TypeList< Ts ... > > {
		using type = TypeList< typename TypeList<Ts ...>::template at<I>::type ... >;
	};
	/*
		template < typename TypeList > struct size;

		template < template < typename ... > class TypeList
				 , typename ... Ts >
		struct size< TypeList< Ts ... >> {
			static constexpr std::size_t value =
					std::integral_constant<std::size_t, sizeof...(Ts)>::value;
		};
	*/
	template < typename ... Ts >
	using size = std::integral_constant< std::size_t, sizeof...(Ts) >;

	template < typename T
		, template < typename ... > class F  >
		struct transform;

	template < template < typename ... > class TypeList
		, template < typename ... > class F
		, typename ... Ts >
		struct transform< TypeList< Ts ... >, F > {
		using type = TypeList< typename F<Ts>::type ... >;
	};
}

#endif
