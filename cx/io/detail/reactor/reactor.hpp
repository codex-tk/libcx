/**
*/
#ifndef __cx_io_detail_reactor_base_h__
#define __cx_io_detail_reactor_base_h__

#include <cx/cxdefine.hpp>
#include <cx/slist.hpp>

#include <cx/io/io.hpp>
#include <cx/io/ip/basic_address.hpp>

namespace cx::io {

    template < typename ImplementationType >
	class reactor_base {
	public:
		struct handle;
		using handle_type = std::shared_ptr<handle>;

		class operation {
		public:
			operation(void)
				: _next(nullptr) {
			}

			virtual ~operation(void) = default;

			int io_size(void) { return _io_size; }
			int io_size(int sz) {
				std::swap(_io_size, sz);
				return sz;
			}

			std::error_code error(void) { return _ec; }
			std::error_code error(const std::error_code& ec) {
				std::error_code old(_ec);
				_ec = ec;
				return old;
			}
			
			operation* next(void) { return _next; }
			operation* next(operation* op) {
				std::swap(_next, op);
				return op;
			}

			virtual int operator()(void) = 0;

			virtual bool complete(const handle_type& /*handle*/){
                return true;
            };
		private:
			std::error_code _ec;
			int _io_size;
			operation* _next;
		};

		using operation_type = operation;
		
		struct handle : public std::enable_shared_from_this<handle> {
			handle( void ){
				fd = -1;
			}

			std::recursive_mutex mutex;
			int fd;
			int interest_events;
			cx::slist<operation> ops[2];

			int handle_events(ImplementationType& impl, int revt) {
				int ops_filter[2] = { cx::io::pollin , cx::io::pollout };
				int proc = 0;
				bool changed = false;
				for (int i = 0; i < 2; ++i) {
					if (ops_filter[i] & revt) {
						operation* op = ops[i].head();
						if (op && op->complete(this->shared_from_this())) {
							ops[i].remove_head();
							proc += (*op)();
							if (ops[i].head() == nullptr)
								changed = true;
						}
					}
				}
				if (changed) {
					int interest = (ops[0].head() ? cx::io::pollin : 0)
						| (ops[1].head() ? cx::io::pollout : 0);
					if (false== impl.bind(this->shared_from_this(), interest)) {
						this->post_all_ops(impl, std::error_code(errno, std::generic_category()));
					}
				}
				return proc;
			}

			void post_all_ops(ImplementationType& impl , const std::error_code& ec ) {
				class _all_ops : public operation {
				public:
					_all_ops(const std::error_code& ec , cx::slist<operation>& ops )
						: _error_code(ec) {
						_ops.swap(ops);
					}

					virtual ~_all_ops(void) {}

					virtual int operator()(void) {
						int proc = 0;
						while (operation_type* op = _ops.head()) {
							ops.remove_head();
							op->error(_error_code);
							proc += (*op)();
						}
						delete this;
						return proc;
					}
				private:
					std::error_code& _error_code;
					cx::slist<operation> _ops;
				};
				cx::slist<operation> o;
				o.add_tail(std::move(this->ops[0]));
				o.add_tail(std::move(this->ops[1]));
				_all_ops* pop = new _all_ops(ec, o);
				impl.post(pop);
			}
		};
	};

}

#endif
