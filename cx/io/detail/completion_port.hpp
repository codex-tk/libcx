/**
 */
#ifndef __cx_io_detail_completion_port_h__
#define __cx_io_detail_completion_port_h__

/*
*/
#include <cx/cxdefine.hpp>
#include <cx/slist.hpp>
#include <cx/container_of.hpp>
#include <cx/error.hpp>
#include <cx/io/base_op.hpp>

#include <mutex>
#include <set>

#if CX_PLATFORM == CX_P_WINDOWS

namespace cx::io::detail {

    class completion_port {
    public:
        struct handle {
            handle( completion_port& arg )
                : implementation( arg ) {}
            union {
                SOCKET s;
                HANDLE h;
            } fd;
            completion_port& implementation;
        };
        using handle_type = std::shared_ptr<completion_port::handle>;

        handle_type make_shared_handle( void ) {
            return std::make_shared< completion_port::handle >( *this );
        }
    public:
        completion_port( void ) 
            :  _handle( CreateIoCompletionPort( INVALID_HANDLE_VALUE , NULL , 0 , 1 ))
        {    
        }

        ~completion_port( void ) {
            CloseHandle(_handle);
		    _handle = INVALID_HANDLE_VALUE;
        }

        bool bind( handle_type& ptr , const int /*ops*/ ) {
            if ( CreateIoCompletionPort(
                ptr->fd.h
                , _handle
                , reinterpret_cast<ULONG_PTR>(ptr.get())
                , 0 ) == _handle )
            {
                _active_handles.insert(ptr);
                return true;
            }
            return false;
        }

        void unbind( handle_type& ptr ) {
            _active_handles.erase( ptr );
        }

        int run( const std::chrono::milliseconds& ms ) {
            LPOVERLAPPED ov = nullptr;
            DWORD bytes_transferred = 0;
            ULONG_PTR key;
            BOOL ret = GetQueuedCompletionStatus( _handle
                , &bytes_transferred
                , &key
                , &ov
                , static_cast<DWORD>(ms.count()));
            if ( ov == nullptr ) {
                cx::slist< base_op > ops;
                {
                    std::lock_guard<std::recursive_mutex> lock(_mutex);
                    _ops.swap(ops);
                }
                int proc = 0;
                while ( base_op* op = ops.head() ) {
                    ops.remove_head();
                    (*op)();
                    delete op;
                    ++proc;
                }
                return proc;
            } else {
                cx::io::base_op* op = cx::container_of( ov , &base_op::_ov );
                if ( FALSE == ret ) {
                    op->error(std::error_code( WSAGetLastError() , cx::windows_category()));
                }
                op->io_size(bytes_transferred);
                (*op)();
                delete op;
                return 1;
            }
        }

        void post( base_op* op ) {
            do {
                std::lock_guard<std::recursive_mutex> lock(_mutex);
                _ops.add_tail(op);
            } while(0);
            PostQueuedCompletionStatus( _handle , 0 , 0 , nullptr );
        }

        template < typename HandlerT >
        void post_handler( HandlerT&& handler ) {
            class handler_op : public base_op {
            public:
                handler_op( HandlerT&& handler ) 
                    : _handler( std::forward<HandlerT>(handler))
                {}
                virtual ~handler_op( void ) override {
                }
                virtual void operator()(void) override {
                    _handler();
                }
            private:
                HandlerT _handler;
            };
            post( new handler_op(std::forward<HandlerT>(handler)));
        }
    private:
        HANDLE _handle;
        std::set< handle_type > _active_handles;
        std::recursive_mutex _mutex;
        cx::slist< base_op > _ops;
    };

}

#endif

#endif

