/**
 */
#ifndef __cx_service_repository_h__
#define __cx_service_repository_h__

#include <cx/core/type_list.hpp>
#include <cx/core/tag.hpp>

namespace cx::internal {

	template < std::size_t I, typename T >
	struct value {
		template < typename U >
		value(U&& u)
			: data(u) {}
		T data;
	};

	template < typename Is, typename ... Ts > struct values;

	template < std::size_t ... Is, typename ... Ts >
	struct values< cx::core::mp::sequence< Is ... >, Ts ... >
		: value< Is, Ts >... {
		template < typename ... Args >
		values(Args&& ... args)
			: value< Is,
			typename cx::core::mp::at< Is, cx::core::type_list< Ts ... > >::type
			>(std::forward<Ts>(args)) ... {}

		template < typename T >
		values(T&& t)
			: value< Is,
			typename cx::core::mp::at< Is, cx::core::type_list< Ts ... > >::type
			>(std::forward<T>(t)) ... {}

		~values(void) {}
	};

}

namespace cx {

	template < typename ... Services >
	class service_repository {
	public:
		template < typename ... Args >
		service_repository(Args&& ... args)
			: _services(std::forward< Args >(args)...) {}

		template < typename T >
		service_repository(T&& t)
			: _services(std::forward<T>(t)) {}

		template < typename T >
		T& service( const cx::tag<T>& ) {
			return _service_impl<T>(_services);
		}
	private:
		template < typename T, std::size_t I >
		T& _service_impl(internal::value< I, T >& t) {
			return t.data;
		}
	private:
		internal::values< cx::core::mp::make_sequence<sizeof...(Services)>
			, Services ... > _services;
	};

}
#endif
