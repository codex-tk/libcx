/**
 * */
#ifndef __cx_io_ip_completion_port_accept_op_h__
#define __cx_io_ip_completion_port_accept_op_h__

#include <cx/io/ip/basic_accept_op.hpp>

namespace cx::io::ip {
    
	template < typename ServiceType, typename HandlerType >
    class completion_port_accept_op : public basic_accept_op< ServiceType > {
    public:
		completion_port_accept_op(ServiceType& svc, HandlerType&& handler)
			: basic_accept_op(svc)
			, _handler(std::forward<HandlerType>(handler)) {}

		virtual ~completion_port_accept_op(void) {}

        virtual int operator()(void) override {
            _handler(error() , accept_context() , address());
			delete this;
			return 1;
        }
    private:
        HandlerType _handler;
    };

}

#endif