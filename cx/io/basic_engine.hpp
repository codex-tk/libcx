/**
 */
#ifndef __cx_io_basic_engine_h__
#define __cx_io_basic_engine_h__

#include <cx/service_repository.hpp>

namespace cx::io {

    template < typename Implementation , typename ... Services >
    class basic_engine {
    public:
        basic_engine( void )
            : _services(_implementation)
        {}
        
        ~basic_engine( void ){
        }

        template < typename Service >
        Service& service( void ) {
            return _services.service< Service >();
        }

        Implementation& implementation( void ) {
            return _implementation;
        }
    private:
        Implementation _implementation;
        cx::service_repository< Services ... > _services;
    };

}

#endif