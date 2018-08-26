/**
 * @brief 
 * 
 * @file descriptor.cpp
 * @author ghtak
 * @date 2018-08-26
 */
#include <cx/io/io.hpp>
#include <cx/io/descriptor.hpp>
#include <cx/io/operation.hpp>

namespace cx::io {

	descriptor::descriptor(void)
		: _fd(-1)
	{

	}

	void descriptor::put(cx::io::type type, cx::io::operation* op) {
		if(type == cx::io::pollin)
			_ops[0].add_tail(op);
		else if(type == cx::io::pollout)
			_ops[1].add_tail(op);
	}

	int descriptor::handle_event(cx::io::engine& , int revt) {
		auto pthis = this->shared_from_this();
		int ops_filter[2] = { cx::io::pollin , cx::io::pollout };
		bool changed = false;
		int ev = 0;
		for (int i = 0; i < 2; ++i) {
			if (ops_filter[i] & revt) {
				operation* op = _ops[i].head();
				if (op && op->is_complete(pthis)) {
					_ops[i].remove_head();
					(*op)();
					if (_ops[i].empty())
						changed = true;
					ev |= ops_filter[i];
				}
			}
		}
		if (changed) {
			int interest = (_ops[0].empty() ? 0 : cx::io::pollin)
				| (_ops[1].empty() ? 0 : cx::io::pollout);
			/*
			if (engine.bind(pthis, interest) == false) {
				this->drain_all_ops(impl, cx::system_error());
			}*/
		}
		return ev;
	}

}