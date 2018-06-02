/**
 * 
 * */

#ifndef __cx_io_ip_acceptor_h__
#define __cx_io_ip_acceptor_h__

#include <cx/io/ip/basic_socket.hpp>
#include <cx/io/ip/option.hpp>
#include <cx/io/selector.hpp>

namespace cx::io::ip
{

/**
     * class HandlerT {
     *      bool select_address( const cx::io::ip::address& addr );
     *      void handle_accept( socket_type fd ,  const cx::io::ip::address& addr  )
     * }
     */
template <typename HandlerT, int Type, int Proto, template <int, int> class SocketLayerT>
class basic_acceptor
{
  public:
    using implementation = SocketLayerT<Type, Proto>;
    basic_acceptor(HandlerT &handler)
        : _handler(handler)
    {
    }
    ~basic_acceptor(void) {}

    bool open(const uint16_t port, const uint16_t family = AF_UNSPEC){
        auto addresses = cx::io::ip::address::any(port, family);
        for (auto addr : addresses){
            if (_handler.select_address(addr)){
                cx::io::ip::basic_socket<Type, Proto, SocketLayerT> fd;
                if (fd.open(addr.family())) {
                    fd.set_option(cx::io::ip::option::reuse_address(cx::io::ip::option::enable));
                    if (addr.family() == AF_INET6)
                        fd.set_option(cx::io::ip::option::bind_ipv6only(cx::io::ip::option::enable));
                    if (fd.bind(addr) && implementation::listen(fd.handle())) {
                        _listen_fds.emplace_back(fd.handle(cx::io::ip::invalid_socket));
                    }
                }
                fd.close();
            }
        }
        for (auto fd : _listen_fds) {
            _selector.bind(fd.handle(), cx::io::ops::read, nullptr);
        }
        return !_listen_fds.empty();
    }
    void close(void) {
        for (auto fd : _listen_fds) {
            _selector.unbind(fd.handle());
            fd.close();
        }
        _listen_fds.clear();
    }

    int accept(  const std::chrono::milliseconds& ms) {
        int cnt = _selector.select( ms.count() );
        if ( cnt > 0 ) {
            selector::iterator it(_selector);
            while ( it ) {
                cx::io::ip::address addr;
                socket_type fd = implementation::accept( it.handle() , addr );
                if ( fd != invalid_socket ) {
                    _handler.handle_accept( fd , addr );
                }
                ++it;
            }
        }
        return cnt;
    }

  private:
    HandlerT &_handler;
    std::vector<cx::io::ip::basic_socket<Type, Proto, SocketLayerT>> _listen_fds;
#if CX_PLATFORM == CX_P_WINDOWS
    using selector = cx::io::selector<SOCKET, 32>;
     #else
    using selector = cx::io::selector<int, 32>;
#endif
    selector _selector;
};

namespace tcp
{
    template < typename HandlerT >
    using acceptor = basic_acceptor< HandlerT , SOCK_STREAM, IPPROTO_TCP, ip::socket_layer >;
}

} // namespace cx::io::ip

#endif