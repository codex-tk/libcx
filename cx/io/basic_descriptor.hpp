/**
 */

#include <cx/io/io.hpp>

namespace cx::io {

    template < typename IoLayerImplT >
    class basic_descriptor {
    public:
        using trait = handle_trait< typename IoLayerImplT::handle_type >;
        using handle_type = trait::handle_type;
        using address = typename IoLayerImplT::address;
        using io_context = typename IoLayerImplT::io_context;

        basic_descriptor( void )   
            : _handle(trait::invalid())
        {
        }

        basic_descriptor( handle_type hande ) 
            : _handle(handle)
        {
        }

        bool open( const address& address ) {
            _handle = IoLayerImplT::open( address );
            return _handle != trait::invalid();
        }

        void close( void ) {
            IoLayerImplT::close( _handle );
        }
        
        int read( io_context& ctx ){
            return IoLayerImplT::read( _fd , ctx );
        }

        int write( io_context& ctx ){
            return IoLayerImplT::write( _fd , ctx );
        }
    private:
        handle_type _handle;
    };

}