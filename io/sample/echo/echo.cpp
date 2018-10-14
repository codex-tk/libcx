#include <iostream>
#include <cx/io/services.hpp>

class session : public std::enable_shared_from_this<session> {
public:
	session(cx::io::engine& e)
		: _fd(e)
		, _read_buffer(1024) {}

	void do_read(void) {
		auto pthis = shared_from_this();
		auto rb = cx::io::buffer(_read_buffer.prepare(1024), 1024);
		_fd.async_recv(rb,
			[&, pthis](const std::error_code& ec, int size) 
		{
			std::cout << "async_recv" << std::endl;
			if (ec || size <= 0) {
				std::cout << "close" << std::endl;
				pthis->socket().close();
				return;
			}
			_read_buffer.commit(size);
			auto wrbuf = cx::deepcopy(_read_buffer);
			auto wb = cx::io::buffer(wrbuf.rdptr(), wrbuf.rdsize());
			_fd.async_send(wb,
				[&, pthis, wrbuf](const std::error_code& , int) {
				std::cout << "async_send" << std::endl;
			});
			_read_buffer.clear();
			do_read();
		});
	}

	cx::io::ip::tcp::socket& socket(void) { return _fd; }
private:
	cx::io::ip::tcp::socket _fd;
	cx::basic_buffer<char> _read_buffer;
};

void async_accept(cx::io::engine& e, cx::io::ip::tcp::acceptor& acceptor) {
	std::shared_ptr<session> ptr(std::make_shared<session>(e));
	acceptor.async_accept(ptr->socket(),
		[&, ptr](const std::error_code& , cx::io::ip::tcp::address& )
	{
		std::cout << "Open" << std::endl;
		ptr->do_read();
		async_accept(e, acceptor);
	});
}

int main(int argc, char* argv[]) {
	CX_UNUSED(argc);
	CX_UNUSED(argv);
	cx::io::engine e;
	cx::io::ip::tcp::acceptor acceptor(e);

	std::error_code ec;
	if (acceptor.open(cx::io::ip::tcp::address::any(7543, AF_INET), ec)) {
		async_accept(e, acceptor);
	}

	while (true)
		e.run(std::chrono::milliseconds(100));

	acceptor.close();
	return 0;
}