/**
 */
#ifndef __cx_io_handler_op_h__
#define __cx_io_handler_op_h__

namespace cx::io {

    template < typename BaseType , typename HandlerType >
    class handler_op : public BaseType {
    public:
        template < typename T >
		handler_op(const T& t, HandlerType&& handler)
			: BaseType(t)
			, _handler(std::forward<HandlerType>(handler)) {}

		handler_op(HandlerType&& handler)
			: _handler(std::forward<HandlerType>(handler)) {}

		virtual ~handler_op(void) {}

		virtual int operator()(void) override {
			_handler(this->error(), this->io_size());
			delete this;
			return 1;
		}
    private:
        HandlerType _handler;
    };

	template < typename BaseType, typename HandlerType >
	class reusable_handler_op : public BaseType {
	public:
		reusable_handler_op(HandlerType&& handler)
			: _handler(std::forward<HandlerType>(handler)) {}

		virtual ~reusable_handler_op(void) {}

		virtual int operator()(void) override {
			_handler(this->error(), this->io_size());
			return 1;
		}
	private:
		HandlerType _handler;
	};

}

#endif