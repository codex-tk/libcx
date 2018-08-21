/**
 * @brief
 *
 * @file type_list.hpp
 * @author ghtak
 * @date 2018-08-19
 */

#ifndef __cx_base_type_list_h__
#define __cx_base_type_list_h__

#include <cx/base/mp.hpp>

namespace cx {

    /**
     * @brief 
     * 
     * @tparam Ts 
     */
	template < typename ... Ts >
	struct type_list {

		using this_type = type_list< Ts ... >;

		template < std::size_t I >
		using at = cx::mp::at<I, this_type>;

		using front = cx::mp::front<this_type>;

		using back = cx::mp::back<this_type>;

		template < typename ... Us >
		using push_back = cx::mp::push_back<this_type, Us ... >;

		template < typename ... Us >
		using push_front = cx::mp::push_front<this_type, Us ... >;

		using pop_front = cx::mp::pop_front<this_type>;

		using pop_back = cx::mp::pop_back< cx::mp::make_sequence<sizeof...(Ts)-1>, this_type>;

		using size = cx::mp::size< Ts ... >;

		template < std::size_t I >
		using at_t = typename cx::mp::at< I, type_list< Ts ... >>::type;

		template < template < typename ... > class U >
		struct rebind {
			using other = U< Ts ... >;
		};
	};
}
#endif