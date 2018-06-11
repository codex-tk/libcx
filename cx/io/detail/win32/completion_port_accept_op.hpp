/**
 * */
#ifndef __cx_io_detail_completion_port_accept_op_h__
#define __cx_io_detail_completion_port_accept_op_h__

#include <cx/io/detail/basic_accept_op.hpp>

namespace cx::io::ip::detail {
    
	template < typename ServiceType, typename HandlerType >
    class completion_port_accept_op : public basic_accept_op< ServiceType > {
    public:
		completion_port_accept_op(const cx::io::ip::basic_socket<ServiceType>& fd, HandlerType&& handler)
			: basic_accept_op(fd)
			, _handler(std::forward<HandlerType>(handler))
		{
		}

        virtual ~completion_port_accept_op( void ) {
        }

        virtual int operator()(void) override {
			socket().service().implementation().bind(socket().handle(), 0);
            _handler(error() , socket() , address());
			delete this;
			return 1;
        }
    private:
        HandlerType _handler;
    };

}

#endif