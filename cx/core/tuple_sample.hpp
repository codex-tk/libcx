/**
 * @brief
 *
 * @file tuple_sample.hpp
 * @author ghtak
 * @date 2018-05-17
 */
#ifndef __cx_core_mp_tuple_sample_h__
#define __cx_core_mp_tuple_sample_h__

#include <cx/core/type_list.hpp>

namespace cx::internal {
	template < std::size_t N, typename T > struct value {
		T data;
		value(void) {}
		value(T&& t) : data(t) {}
	};

	template < typename I, typename ... Ts > struct values;

	template < std::size_t ... Is, typename ... Ts >
	struct values< cx::sequence< Is ... >, Ts ...  >
		: value< Is, Ts > ...
	{
		values(Ts&& ...  args)
			: value< Is, typename cx::at< Is, type_list< Ts ... >>::type >(std::forward<Ts>(args)) ...
		{}

		template < std::size_t I >
		constexpr typename cx::at< I, type_list< Ts ... >>::type
			operator[](const std::integral_constant< std::size_t, I >&)
		{
			return static_cast<value< I, typename cx::at< I, type_list< Ts ... >>::type >*>(this)->data;
		}
	};

	template < typename I, typename T > struct values0;

	template < std::size_t ... Is, typename ... Ts >
	struct values0< cx::sequence< Is ... >, type_list< Ts ... > >
		: value< Is, typename cx::at< Is, type_list< Ts ... >>::type > ...
	{
		values0(Ts&& ...  args)
			: value< Is, typename cx::at< Is, type_list< Ts ... >>::type >(std::forward<Ts>(args)) ...
		{}

	};

	template < std::size_t I, typename T >
	T& get(value< I, T >& v) {
		return v.data;
	}

	template < typename ... Ts >
	using tuple_sample = values< cx::make_sequence< sizeof...(Ts) >, Ts ... >;
}

#endif