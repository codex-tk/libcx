/**
 * @brief 
 * 
 * @file tuple.hpp
 * @author ghtak
 * @date 2018-08-19
 */

#ifndef __cx_base_mp_tuple_sample_h__
#define __cx_base_mp_tuple_sample_h__

#include <cx/base/type_list.hpp>

namespace cx::base::mp {
    
	template < std::size_t N, typename T > 
	struct value {
		T data;
		value(void) {}
		value(T&& t) : data(t) {}
	};

	template < typename I, typename ... Ts > 
	struct values;

	template < std::size_t ... Is, typename ... Ts >
	struct values< cx::base::mp::sequence< Is ... >, Ts ...  >
		: value< Is, Ts > ...
	{
		values(Ts&& ...  args)
			: value< Is, typename cx::base::mp::at< Is, type_list< Ts ... >>::type >(std::forward<Ts>(args)) ...
		{}

		template < std::size_t I >
		constexpr typename cx::base::mp::at< I, type_list< Ts ... >>::type
			operator[](const std::integral_constant< std::size_t, I >&)
		{
			return static_cast<value< I, typename cx::base::mp::at< I, type_list< Ts ... >>::type >*>(this)->data;
		}
	};

	template < typename I, typename T > struct values0;

	template < std::size_t ... Is, typename ... Ts >
	struct values0< cx::base::mp::sequence< Is ... >, type_list< Ts ... > >
		: value< Is, typename cx::base::mp::at< Is, type_list< Ts ... >>::type > ...
	{
		values0(Ts&& ...  args)
			: value< Is, typename cx::base::mp::at< Is, type_list< Ts ... >>::type >(std::forward<Ts>(args)) ...
		{}

	};

	template < std::size_t I, typename T >
	T& get(value< I, T >& v) {
		return v.data;
	}

	template < typename ... Ts >
	using tuple_sample = values< cx::base::mp::make_sequence< sizeof...(Ts) >, Ts ... >;
}

#endif