/**
 * @brief 
 * 
 * @file basic_connect.hpp
 * @author ghtak
 * @date 2018-06-03
 */

#ifndef __cx_io_ip_basic_connect_h__
#define __cx_io_ip_basic_connect_h__

#include <cx/io/base_op.hpp>
#include <cx/io/ip/basic_address.hpp>

namespace cx::io { class engine; }
namespace cx::io::ip::detail {
    
    class base_connect : public base_op {
    public:
        base_connect( const cx::io::ip::address& address )
            : _address(address) 
        {
        }
        virtual ~base_connect( void ){}

        virtual int type(void) = 0;
        virtual int proto(void) = 0;
        cx::io::ip::address& address( void ) {
            return _address;
        }
    private:
        cx::io::ip::address _address;
    };

    template < typename HandlerT , int Type , int Proto >
    class basic_connect : public base_connect {
    public:
        basic_connect( const cx::io::ip::address& address , HandlerT&& h ) 
            : base_connect(address) 
            , _handler( std::forward<HandlerT>(h)) 
        {
        }

        virtual ~basic_connect( void ) {
        }

        virtual void operator()(void) override {
            _handler(error());
        }

        virtual int type(void) override {
            return Type;
        }
        virtual int proto(void) override {
            return Proto;
        }
    private:
        HandlerT _handler;
    };

}

#endif