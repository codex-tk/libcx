/**
 */
#ifndef __cx_io_basic_write_op_h__
#define __cx_io_basic_write_op_h__

namespace cx::io {

    template < typename ServiceType >
    class base_write_op : public ServiceType::operation_type {
    public:
        using buffer_type = typename ServiceType::buffer_type;

        base_write_op( const buffer_type& buf )
            : _buffer(buf)
        {}

        buffer_type& buffer( void ) {
            return _buffer;
        }
    private:
        buffer_type _buffer;
    };

    template < typename ServiceType , typename HandlerType >
    class basic_write_op : public base_write_op< ServiceType > {
    public:
		basic_write_op(const buffer_type& buf, HandlerType&& handler)
			: base_write_op(buf)
			, _handler(std::forward<HandlerType>(handler))
		{
		}

        virtual ~basic_write_op( void ) {
        }

        virtual void operator()(void) override {
            _handler(error(),io_size());
        }
    private:
        HandlerType _handler;
    };

}

#endif