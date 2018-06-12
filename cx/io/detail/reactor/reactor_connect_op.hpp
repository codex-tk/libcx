/**
 * */
#ifndef __cx_io_ip_reactor_connect_op_h__
#define __cx_io_ip_reactor_connect_op_h__

#include <cx/io/ip/basic_connect_op.hpp>

namespace cx::io::ip {

    template < typename ServiceType , typename HandlerType >
    class reactor_connect_op : public cx::io::ip::basic_connect_op< ServiceType > {
    public:
        using handle_type = typename ServiceType::handle_type;
        using address_type = typename ServiceType::address_type;

		reactor_connect_op(const address_type& addr, HandlerType&& handler)
			: basic_connect_op< ServiceType >(addr)
			, _handler(std::forward<HandlerType>(handler))
		{
		}

        virtual ~reactor_connect_op( void ){
        }

        virtual int operator()(void) override {
            _handler(this->error());
			delete this;
			return 1;
        }
    private:
        HandlerType _handler;
    };

}

#endif
