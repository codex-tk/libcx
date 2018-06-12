/**
 * */
#ifndef __cx_io_ip_reactor_write_op_h__
#define __cx_io_ip_reactor_write_op_h__

#include <cx/io/basic_write_op.hpp>

namespace cx::io {

    template < typename ServiceType , typename HandlerType >
    class reactor_write_op : public cx::io::basic_write_op< ServiceType > {
    public:
        using buffer_type = typename ServiceType::buffer_type; 
        using handle_type = typename ServiceType::handle_type;
        using implementation_type = typename ServiceType::implementation_type;
		reactor_write_op(const buffer_type& buf, HandlerType&& handler)
			: basic_write_op<ServiceType>(buf)
			, _handler(std::forward<HandlerType>(handler))
		{
		}
        
        virtual ~reactor_write_op( void ) {
        }

        virtual bool complete( const typename reactor_base<implementation_type>::handle_type& handle ) override {
             handle_type ptr = std::static_pointer_cast< typename handle_type::element_type >(handle);
             return ptr->service.write_complete( ptr , this );
        }

        virtual int operator()(void) override {
            _handler(this->error(),this->io_size());
			delete this;
			return 1;
        }
    private:
        HandlerType _handler;
    };

}

#endif
