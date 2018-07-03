/**
 */
#ifndef __cx_io_iocp_implementation_h__
#define __cx_io_iocp_implementation_h__

 /*
 */
#include <cx/cxdefine.hpp>
#include <cx/slist.hpp>
#include <cx/container_of.hpp>
#include <cx/error.hpp>

#include <cx/io/internal/basic_implementation.hpp>

#include <mutex>
#include <set>

#if CX_PLATFORM == CX_P_WINDOWS

namespace cx::io::internal::iocp {

	class implementation : public cx::io::internal::basic_implementation {
	public:
		struct basic_handle : std::enable_shared_from_this<basic_handle> {
			union {
				SOCKET s;
				HANDLE h;
			} fd;
			basic_handle(void) noexcept { fd.h = INVALID_HANDLE_VALUE; }
		};

		using handle_type = std::shared_ptr<basic_handle>;

		class operation : public OVERLAPPED {
		public:
			operation(void) noexcept
				: _next(nullptr) {
				reset();
			}

			virtual ~operation(void) = default;
			
			int io_size(void) { return this->Offset; }
			int io_size(int sz) {
				int old = this->Offset;
				this->Offset = sz;
				sz = old;
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

			virtual bool complete(const handle_type& /*handle*/) {
				return true;
			};

			OVERLAPPED *overlapped(void) { return static_cast<OVERLAPPED*>(this); }
			void reset(void) { memset(overlapped(), 0x00, sizeof(decltype(*overlapped()))); }
		private:
			std::error_code _ec;
			operation* _next;
		};
		using handle_type = std::shared_ptr<implementation::basic_handle>;
		using operation_type = operation;
	public:
		implementation(void)
			: _handle(CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1))
			, _active_links(0) {
			if (_handle == INVALID_HANDLE_VALUE) {
				throw std::system_error(cx::system_error() , "CreateIoCompletionPort fails" );
			}
		}

		~implementation(void) {
			CloseHandle(_handle);
			_handle = INVALID_HANDLE_VALUE;
		}

		bool bind(const handle_type& ptr, const int /*ops*/) {
			if (ptr.get() == nullptr || ptr->fd.h == INVALID_HANDLE_VALUE) {
				this->last_error(std::make_error_code(std::errc::invalid_argument));
				return false;
			}
				
			if (CreateIoCompletionPort(
				ptr->fd.h
				, _handle
				, reinterpret_cast<ULONG_PTR>(ptr.get())
				, 0) != _handle)
			{
				this->last_error(cx::system_error());
				return false;
			}
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			_active_handles.insert(ptr);
			return true;
			
		}

		void unbind(const handle_type& ptr) {
			if (ptr.get() == nullptr)
				return;
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			_active_handles.erase(ptr);
		}

		void add_active_links(void) {
			_active_links += 1;
		}

		void release_active_links(void) {
			_active_links -= 1;
		}

		void run(void) {
			while (_active_links.load() != 0 || _active_handles.size() != 0) {
				run(std::chrono::milliseconds(0xffffffff));
			}
		}

		int run(const std::chrono::milliseconds& ms) {
			LPOVERLAPPED ov = nullptr;
			DWORD bytes_transferred = 0;
			ULONG_PTR key;
			BOOL ret = GetQueuedCompletionStatus(_handle
				, &bytes_transferred
				, &key
				, &ov
				, static_cast<DWORD>(ms.count()));
			cx::io::internal::basic_implementation::scoped_loop sl(*this);
			if (ov == nullptr) {
				cx::slist< operation_type > ops;
				do {
					std::lock_guard<std::recursive_mutex> lock(_mutex);
					_ops.swap(ops);
					release_active_links();
				} while(0);
				int proc = 0;
				while (operation_type* op = ops.head()) {
					ops.remove_head();
					proc += (*op)();
				}
				return proc;
			} else {
				operation_type* op = static_cast<operation_type*>(ov);
				implementation::basic_handle* pbasic_handle =
					reinterpret_cast<implementation::basic_handle*>(key);
				if (FALSE == ret) {
					op->error( cx::system_error() );
				}
				op->io_size(bytes_transferred);
				if(op->complete( pbasic_handle->shared_from_this()))
					(*op)();
				release_active_links();
			}
			return 1;
		}

		void post(operation_type* op) {
			do {
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				bool empty = _ops.empty();
				_ops.add_tail(op);
				if (empty) {
					add_active_links();
				}
			} while (0);
			PostQueuedCompletionStatus(_handle, 0, 0, nullptr);
		}

		void post(cx::slist<operation_type>&& ops) {
			do {
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				bool empty = _ops.empty();
				_ops.add_tail(std::forward<cx::slist<operation_type>>(ops));
				if (empty) {
					add_active_links();
				}
			} while (0);
			PostQueuedCompletionStatus(_handle, 0, 0, nullptr);
		}

		template < typename HandlerT >
		void post_handler(HandlerT&& handler) {
			class handler_op : public operation_type {
			public:
				handler_op(HandlerT&& handler)
					: _handler(std::forward<HandlerT>(handler))
				{}
				virtual ~handler_op(void) override {
				}
				virtual int operator()(void) override {
					_handler();
					delete this;
					return 1;
				}
			private:
				HandlerT _handler;
			};
			post(new handler_op(std::forward<HandlerT>(handler)));
		}
	public:
		HANDLE _handle;
		std::recursive_mutex _mutex;
		cx::slist<operation_type> _ops;
		std::set<handle_type> _active_handles;
		std::atomic<int> _active_links;
	};

}

#endif

#endif

