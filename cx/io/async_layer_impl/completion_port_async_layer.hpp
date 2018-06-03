/**
 * @brief 
 * 
 * @file completion_port_async_layer.hpp
 * @author ghtak
 * @date 2018-06-03
 */
#ifndef __cx_io_detail_completion_port_async_layer_h__
#define __cx_io_detail_completion_port_async_layer_h__

#include <memory>
#include <cx/cxdefine.hpp>
#include <cx/error.hpp>
#include <cx/io/ip/option.hpp>
#include <cx/io/async_layer_impl/basic_connect.hpp>
#include <cx/container_of.hpp>

#include <set>

#if CX_PLATFORM == CX_P_WINDOWS

namespace cx::io { class engine; }
namespace cx::io::detail {
      
    class completion_port_async_layer {
    public:
        struct handle;
        using handle_ptr = std::shared_ptr< completion_port_async_layer::handle >;

        completion_port_async_layer( cx::io::engine& e ) 
            : _handle( CreateIoCompletionPort( INVALID_HANDLE_VALUE , NULL , 0 , 1 ))
            , _engine( e ){
            
        }

        ~completion_port_async_layer( void ) {
            CloseHandle(_handle);
		    _handle = INVALID_HANDLE_VALUE;
        }


        handle_ptr make_shared_handle( void ) {
            return std::make_shared< completion_port_async_layer::handle >( *this );
        }

        struct handle{
            handle( completion_port_async_layer& l ) : layer(l){}
            SOCKET fd;
            completion_port_async_layer& layer;
        };
        

        void connect( handle_ptr ptr , cx::io::ip::detail::base_connect * op ) {
            ptr->fd = ::WSASocketW( op->address().family() 
                        , op->type() 
                        , op->proto() 
                        , nullptr
                        , 0 
                        , WSA_FLAG_OVERLAPPED );
            if ( INVALID_SOCKET != ptr->fd ) {
                cx::io::ip::option::non_blocking nb;
                nb.set(ptr->fd);
                LPFN_CONNECTEX _connect_ex = nullptr;
                DWORD bytes_returned = 0;
                GUID guid = WSAID_CONNECTEX;
                if (SOCKET_ERROR != WSAIoctl(ptr->fd
                    , SIO_GET_EXTENSION_FUNCTION_POINTER
                    , &guid, sizeof(guid)
                    , &_connect_ex, sizeof(_connect_ex)
                    , &bytes_returned, nullptr, nullptr)) {
                    cx::io::ip::address addr = cx::io::ip::address::any( 0 , op->address().family());
                    if ( ::bind( ptr->fd , addr.sockaddr() , addr.length()) != SOCKET_ERROR ) {
                        if ( this->bind( ptr )) {
                            memset( op->overlapped() , 0x00 , sizeof(decltype(op->overlapped())));
                            bytes_returned = 0;
                            if ( _connect_ex(ptr->fd
                                , op->address().sockaddr() 
                                , op->address().length() 
                                , nullptr , 0 
                                , &bytes_returned , op->overlapped() ) == TRUE ) {
                                return;
                            }
                        }
                    }
                }
            }
            op->error( std::error_code( WSAGetLastError() , cx::windows_category()));
        }

        void closesocket( handle_ptr& ptr ) {
            ::closesocket( ptr->fd );
            ptr->fd = INVALID_SOCKET;
            unbind( ptr );
        }
        cx::io::engine& engine( void ) {
            return _engine;
        }

        bool bind( handle_ptr& ptr ) {
            if ( CreateIoCompletionPort(
                reinterpret_cast<HANDLE>(ptr->fd)
                , _handle
                , reinterpret_cast<ULONG_PTR>(ptr.get())
                , 0 ) == _handle )
            {
                _handles.insert(ptr);
                return true;
            }
            return false;
        }

        void unbind( handle_ptr& ptr ) {
            _handles.erase( ptr );
        }

        int run( const const std::chrono::milliseconds& ms ) {
            LPOVERLAPPED ov;
            DWORD bytes_transferred = 0;
            ULONG_PTR key;
            BOOL ret = GetQueuedCompletionStatus( _handle
                , &bytes_transferred
                , &key
                , &ov
                , static_cast<DWORD>(ms.count()));
            if ( ov == nullptr ) {
                return 0;
            } else {
                cx::io::base_op* op = cx::container_of( ov , &base_op::_ov );
                if ( FALSE == ret ) {
                    op->error(std::error_code( WSAGetLastError() , cx::windows_category()));
                }
                op->io_size(bytes_transferred);
                (*op)();
                return 1;
            }
        }
    private:
        cx::io::engine& _engine;
        HANDLE _handle;
        std::set< handle_ptr > _handles;
    };
}
#endif

#endif