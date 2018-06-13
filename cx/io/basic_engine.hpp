/**
 */
#ifndef __cx_io_basic_engine_h__
#define __cx_io_basic_engine_h__

#include <cx/service_repository.hpp>
#include <cx/services.hpp>

namespace cx::io {

	template < typename ImplementationType, typename ... Services >
	class basic_engine {
	public:
		using implementation_type = ImplementationType;
		basic_engine(void)
			: _services(_implementation)
		{}

		~basic_engine(void) {
		}

        template < typename ServiceType >
        ServiceType& service(const cx::tag<ServiceType>& tag ) {
            return this->_services.service( tag );
        }

		implementation_type& implementation(void) {
			return _implementation;
		}

		void run(void) {
			_implementation.run();
		}
	private:
		implementation_type _implementation;
		cx::service_repository< Services ... > _services;
	};

	template < typename ... Services >
	using engine = basic_engine< cx::io::implementation, Services ... >;
}

#endif
