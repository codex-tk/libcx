/**
 * */
#ifndef __cx_io_ip_reactor_write_op_h__
#define __cx_io_ip_reactor_write_op_h__

#include <cx/io/basic_write_op.hpp>

namespace cx::io {

    template < typename ServiceType , typename HandlerType >
    class reactor_write_op : public cx::io::basic_write_op< ServiceType > {
    public:
		reactor_write_op(const address_type& addr, HandlerType&& handler)
			: basic_write_op(addr)
			, _handler(std::forward<HandlerType>(handler))
		{
		}
        
        virtual ~reactor_write_op( void ) {
        }

        virtual bool complete( const basic_reactor::handle_type& handle ) override {
             handle_type ptr = std::static_pointer_cast< typename handle_type::element_type >(handle);
             return ptr->service.write_complete( ptr , this );
        }


        virtual int operator()(void) override {
            _handler(error());
			delete this;
			return 1;
        }
    private:
        HandlerType _handler;
    };

}

#endif
