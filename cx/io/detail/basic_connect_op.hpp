/**
 */
#ifndef __cx_io_ip_detail_basic_connect_op_h__
#define __cx_io_ip_detail_basic_connect_op_h__

namespace cx::io::ip{

    template < typename ServiceType >
    class basic_connect_op : public ServiceType::operation_type {
    public:
        using address_type = typename ServiceType::address_type;

        basic_connect_op( const address_type& addr )
            : _address(addr)
        {}

        address_type& address( void ) {
            return _address;
        }
    private:
        address_type _address;
    };

}

#endif