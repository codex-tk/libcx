/**
 * */
#ifndef __cx_io_ip__accept_op_h__
#define __cx_io_ip__accept_op_h__

#include <cx/io/ip/basic_accept_op.hpp>

namespace cx::io::internal::iocp::ip {
    
	template < typename ServiceType, typename HandlerType >
    class accept_op : public cx::io::ip::basic_accept_op< ServiceType > {
    public:
		accept_op(HandlerType&& handler)
			: _handler(std::forward<HandlerType>(handler)) {}

		virtual ~accept_op(void) {}

        virtual int operator()(void) override {
		    _handler(error() , raw_handle() , address());
			delete this;
			return 1;
        }
    private:
        HandlerType _handler;
    };

}

#endif