/**
 */
#ifndef __cx_io_basic_write_op_h__
#define __cx_io_basic_write_op_h__

namespace cx::io {

    template < typename ServiceType >
    class basic_write_op : public ServiceType::operation_type {
    public:
        using buffer_type = typename ServiceType::buffer_type;

		basic_write_op(void) noexcept {}

		basic_write_op(const buffer_type& buf)
			: _buffer(buf) {}

        buffer_type& buffer( void ) {
            return _buffer;
        }
    private:
        buffer_type _buffer;
    };
}

#endif