/**
 * */
#ifndef __cx_io_ip_reactor_read_op_h__
#define __cx_io_ip_reactor_read_op_h__

#include <cx/io/basic_read_op.hpp>

namespace cx::io  {

    template < typename ServiceType , typename HandlerType >
    class reactor_read_op : public cx::io::basic_read_op< ServiceType > {
    public:
		reactor_read_op(const address_type& addr, HandlerType&& handler)
			: basic_read_op(addr)
			, _handler(std::forward<HandlerType>(handler))
		{
		}
        
        virtual ~reactor_read_op( void ) {
        }

        virtual bool complete( const basic_reactor::handle_type& handle ) override {
             handle_type ptr = std::static_pointer_cast< typename handle_type::element_type >(handle);
             return ptr->service.read_complete( ptr , this );
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
