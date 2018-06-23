/**
 */
#ifndef __cx_service_repository_h__
#define __cx_service_repository_h__

#include <cx/core/type_list.hpp>
#include <cx/core/tag.hpp>

namespace cx::internal {
	/*
	template < std::size_t I, typename T >
	struct value {
		template < typename U >
		value(U&& u)
			: data(u) {}
		T data;
	};

	template < typename Is, typename ... Ts > struct values;

	template < std::size_t ... Is, typename ... Ts >
	struct values< cx::mp::sequence< Is ... >, Ts ... >
		: value< Is, Ts >... {
		template < typename ... Args >
		values(Args&& ... args)
			: value< Is,
			typename cx::mp::at< Is, cx::type_list< Ts ... > >::type
			>(std::forward<Ts>(args)) ... {}

		template < typename T >
		values(T&& t)
			: value< Is,
			typename cx::mp::at< Is, cx::type_list< Ts ... > >::type
			>(std::forward<T>(t)) ... {}

		~values(void) {}
	};
	*/

	template < typename T > struct storage {
		template < typename U > storage(U&& u) : data(u) {}
		T data;
	};
	template < typename ... Ts > struct combined_storage : storage<Ts> ... {
		template < typename ... Args > combined_storage(Args&& ... args)
			: storage< Ts >(args)... {}

		template < typename T > combined_storage(T&& t)
			: storage< Ts >(t)...{}

		template < typename T > T& _data(storage<T>& s) { return s.data; }
		template < typename T > T& data(void) {
			return combined_storage::template _data<T>(*this);
		}
	};
}

namespace cx {

	using tt = decltype(std::declval<int>());
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
		T& service(void) {
			return _services.template data<T>();
		}
	private:
		/*
		template < typename T > T& service( void ) {
			return _service_impl<T>(_services);
		}
		template < typename T, std::size_t I > T& _service_impl(internal::value< I, T >& t) {
			return t.data;
		}
		internal::values< cx::mp::make_sequence<sizeof...(Services)>
			, Services ... > _services;*/
		internal::combined_storage< Services... > _services;
	};

}
#endif
