/**
 */
#ifndef __cx_io_handler_op_h__
#define __cx_io_handler_op_h__

namespace cx::io {

    template < typename BaseOpType , typename HandlerType >
    class handler_op : public BaseOpType {
    public:
        template < typename T >
	    handler_op(const T& t, HandlerType&& handler)
			: BaseOpType(t)
			, _handler(std::forward<HandlerType>(handler))
		{
		}

		virtual ~handler_op(void) {
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