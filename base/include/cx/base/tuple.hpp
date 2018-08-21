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

namespace cx::mp {
    
	template < std::size_t N, typename T > 
	struct value {
		T data;
		value(void) {}
		value(T&& t) : data(t) {}
	};

	template < typename I, typename ... Ts > 
	struct values;

	template < std::size_t ... Is, typename ... Ts >
	struct values< cx::mp::sequence< Is ... >, Ts ...  >
		: value< Is, Ts > ...
	//	: value< Is, typename cx::mp::at< Is, type_list< Ts ... >>::type > ...
	{
		values(Ts&& ... args)
			: value< Is, typename cx::mp::at< Is, type_list< Ts ... >>::type >(std::forward<Ts>(args)) ...
		{}

		template < std::size_t I >
		constexpr typename cx::mp::at< I, type_list< Ts ... >>::type
			operator[](const std::integral_constant< std::size_t, I >&)
		{
			return static_cast<value< I, typename cx::mp::at< I, type_list< Ts ... >>::type >*>(this)->data;
		}
	};
	
	template < std::size_t I, typename T >
	T& get(value< I, T >& v) {
		return v.data;
	}

	template < typename ... Ts >
	using tuple = values< cx::mp::make_sequence< sizeof...(Ts) >, Ts ... >;
}

#endif