/**
 * @brief
 *
 * @file type_list.hpp
 * @author ghtak
 * @date 2018-05-12
 */

#ifndef __cx_core_type_list_h__
#define __cx_core_type_list_h__

#include <cx/core/mp.hpp>

namespace cx {

	template < typename ... Ts >
	struct type_list {

		using self = type_list< Ts ... >;

		template < std::size_t I >
		using at = cx::at< I, self >;

		using front = cx::front< self>;

		using back = cx::back< self>;

		template < typename ... Us >
		using push_back = cx::push_back< self, Us ... >;

		template < typename ... Us >
		using push_front = cx::push_front< self, Us ... >;

		using pop_front = cx::pop_front< self >;

		using pop_back = cx::pop_back< cx::make_sequence<sizeof...(Ts)-1>, self >;

		using size = cx::size< Ts ... >;

		template < std::size_t I >
		using at_t = typename cx::at< I, type_list< Ts ... >>::type;

		template < template < typename ... > class U >
		struct rebind {
			using other = U< Ts ... >;
		};
	};
}
#endif