/**
 */
#ifndef __cx_io_ip_detail_basic_connect_op_h__
#define __cx_io_ip_detail_basic_connect_op_h__

namespace cx::io::ip{

    template < typename ServiceType >
    class base_connect_op : public ServiceType::operation_type {
    public:
        using address_type = typename ServiceType::address_type;

        base_connect_op( const address_type& addr )
            : _address(addr)
        {}

        address_type& address( void ) {
            return _address;
        }
    private:
        address_type _address;
    };

    template < typename ServiceType , typename HandlerType >
    class basic_connect_op : public base_connect_op< ServiceType > {
    public:
		basic_connect_op(const address_type& addr, HandlerType&& handler)
			: base_connect_op(addr)
			, _handler(std::forward<HandlerType>(handler))
		{
		}

        virtual ~basic_connect_op( void ) {
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