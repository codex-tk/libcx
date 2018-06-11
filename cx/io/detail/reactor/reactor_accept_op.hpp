
/**
 * */
#ifndef __cx_io_ip_reactor_accept_op_h__
#define __cx_io_ip_reactor_accept_op_h__

#include <cx/io/ip/basic_accept_op.hpp>

namespace cx::io::ip {
    
	template < typename ServiceType, typename HandlerType >
    class reactor_accept_op : public basic_accept_op< ServiceType > {
    public:
		reactor_accept_op(const cx::io::ip::basic_socket<ServiceType>& fd, HandlerType&& handler)
			: basic_accept_op(fd)
			, _handler(std::forward<HandlerType>(handler))
		{
		}

        virtual ~reactor_accept_op( void ) {
        }

        virtual bool complete( const basic_reactor::handle_type& handle ) override {
            handle_type ptr = std::static_pointer_cast< typename handle_type::element_type >(handle);
            return ptr->service.accept_complete( ptr , this );
        }


        virtual int operator()(void) override {
            _handler(error() , socket() , address());
			delete this;
			return 1;
        }
    private:
        HandlerType _handler;
    };

}

#endif