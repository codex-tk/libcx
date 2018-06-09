/**
*/
#ifndef __cx_io_basic_read_op_h__
#define __cx_io_basic_read_op_h__

namespace cx::io {

	template < typename ServiceType >
	class base_read_op : public ServiceType::operation_type {
	public:
		using buffer_type = typename ServiceType::buffer_type;

		base_read_op(const buffer_type& buf)
			: _buffer(buf)
		{}

		buffer_type& buffer(void) {
			return _buffer;
		}
	private:
		buffer_type _buffer;
	};

	template < typename ServiceType, typename HandlerType >
	class basic_read_op : public base_read_op< ServiceType > {
	public:
		basic_read_op(const buffer_type& buf, HandlerType&& handler)
			: base_read_op(buf)
			, _handler(std::forward<HandlerType>(handler))
		{
		}

		virtual ~basic_read_op(void) {
		}

		virtual int operator()(void) override {
			_handler(error(), io_size());
			delete this;
			return 1;
		}
	private:
		HandlerType _handler;
	};

}

#endif