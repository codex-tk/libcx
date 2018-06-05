/**
 */
#ifndef __cx_io_basic_object_h__
#define __cx_io_basic_object_h__

namespace cx::io {

    template < typename ServiceType >
    class basic_object{
    public:
        using implementation_type = typename ServiceType::implementation_type;
        using handle_type = typename ServiceType::handle_type;
        using address_type = typename ServiceType::address_type;
        
        template < typename EngineType >
        basic_object( EngineType& engine )
            : _handle( engine.service<ServiceType>()
                    .implementation()
                    .make_shared_handle())
        {
        }

        handle_type handle( void ) {
            return _handle;
        }
    private:
        handle_type _handle;
    };

}

#endif