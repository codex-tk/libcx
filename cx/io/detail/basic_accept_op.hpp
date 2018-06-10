/**
 */
#ifndef __cx_io_ip_detail_basic_accept_op_h__
#define __cx_io_ip_detail_basic_accept_op_h__

#include <cx/io/ip/basic_socket.hpp>

namespace cx::io::ip{

    template < typename ServiceType >
	class base_accept_op : public ServiceType::operation_type {
	public:
		using handle_type = typename ServiceType::handle_type;
		using address_type = typename ServiceType::address_type;

		base_accept_op(const cx::io::ip::basic_socket<ServiceType>& fd)
			: _socket(fd)
		{}

		cx::io::ip::basic_socket<ServiceType>& socket(void) {
			return _socket;
		}
		address_type& address(void) {
			return _address;
		}
	private:
		address_type _address;
		cx::io::ip::basic_socket<ServiceType> _socket;
	};

	template < typename ServiceType, typename HandlerType >
    class basic_accept_op : public base_accept_op< ServiceType > {
    public:
		basic_accept_op(const cx::io::ip::basic_socket<ServiceType>& fd, HandlerType&& handler)
			: base_accept_op(fd)
			, _handler(std::forward<HandlerType>(handler))
		{
		}

        virtual ~basic_accept_op( void ) {
        }

        virtual int operator()(void) override {
#if CX_PLATFORM == CX_P_WINDOWS
			socket().service().implementation().bind(socket().handle(), 0);
#endif
            _handler(error() , socket() , address());
			delete this;
			return 1;
        }
    private:
        HandlerType _handler;
    };

}

#endif