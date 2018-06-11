/**
 * */
#ifndef __cx_io_ip_completion_port_connect_op_h__
#define __cx_io_ip_completion_port_connect_op_h__

#include <cx/io/ip/basic_connect_op.hpp>

namespace cx::io::ip {

    template < typename ServiceType , typename HandlerType >
    class completion_port_connect_op : public basic_connect_op< ServiceType > {
    public:
		completion_port_connect_op(const address_type& addr, HandlerType&& handler)
			: basic_connect_op(addr)
			, _handler(std::forward<HandlerType>(handler))
		{
		}

        virtual ~completion_port_connect_op( void ) {
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