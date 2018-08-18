#include <cx/io/basic_engine.hpp>
#include <cx/core/basic_buffer.hpp>
#include <iostream>

#include <cx/io/ip/hdr.hpp>

using engine_type = cx::io::engine<
	cx::io::ip::icmp::service
	, cx::time::timer_service >;

class session : public std::enable_shared_from_this<session> {
public:
	session(engine_type& e)
		: _fd(e), _timer(e), _read_buffer(1024) {}

	bool start_ping(const std::string& ip, int times) {
		_ip_address = ip;
		_current_times = 0;
		_times = times;

		if (_fd.open(cx::io::ip::icmp::address::any(0, AF_INET))) {
			init_timer();
			start_timer();
			send_ping();
			do_read();
			return true;
		}
		return false;
	}

	void do_read(void) {
		if (!_read_op) {
			auto pthis = shared_from_this();
			_read_op = _fd.make_read_op([&, pthis](const std::error_code& ec, int size) {
				if (ec || size <= 0) {
					std::cout << "socket error :" << ec.message() << std::endl;
					_fd.close();
					_timer.cancel();
					return;
				}
				_read_buffer.commit(size);
				cx::io::ip::iphdr* piphdr = reinterpret_cast<cx::io::ip::iphdr*>(_read_buffer.rdptr());
				int offset = piphdr->ip_hl * 4;
				_read_buffer.rdptr(offset);
				if (_read_buffer.rdsize() > 0) {
					cx::io::ip::icmphdr* picmphdr = reinterpret_cast<cx::io::ip::icmphdr*>(_read_buffer.rdptr());
					printf("Type : %d \n", picmphdr->icmp_type);
					printf("Code : %d \n", picmphdr->icmp_code);
					printf("Seq  : %d \n", picmphdr->icmp_sequence);
					printf("Iden : %d \n", picmphdr->icmp_id);
				}
				do_read();
			});
		}
		_read_buffer.clear();
		_read_op->buffer().buffer.reset(_read_buffer.prepare(1024), 1024);
		_fd.async_read(_read_op);
	}
	
	void init_timer(void) {
		auto pthis = shared_from_this();
		_timer.handler([&, pthis](const std::error_code& ec) {
			if (ec == std::errc::operation_canceled) {
				std::cout << "canceled" << std::endl;
				return;
			}
			++_current_times;
			if (_current_times < _times  &&  _fd.good()) {
				send_ping();
				start_timer();
			}
			else {
				std::cout << "timer close" << std::endl;
				_fd.close();
			}
		});
	}

	void start_timer() {
		_timer.expired_at(std::chrono::system_clock::now() + std::chrono::seconds(1));
		_timer.fire();
	}

	void send_ping(void) {
		cx::io::ip::icmphdr icmphdr;
		cx::io::ip::icmp::buffer wrbuf;
		
		wrbuf.address = cx::io::ip::icmp::address(_ip_address.c_str(), 0, AF_INET);
		wrbuf.buffer = cx::io::buffer(&icmphdr, sizeof(icmphdr));

		icmphdr.icmp_type = 8; //ICMP_ECHO;
		icmphdr.icmp_code = 0;
		icmphdr.icmp_checksum = 0;
		icmphdr.icmp_sequence = _current_times;
		icmphdr.icmp_id = 81;
		icmphdr.icmp_checksum = cx::checksum(&icmphdr, sizeof(icmphdr));
		_fd.write(wrbuf);
	}
private:
	cx::io::ip::icmp::socket _fd;
	cx::io::ip::icmp::socket::read_op_type _read_op;
	cx::time::timer _timer;
	cx::basic_buffer<char> _read_buffer;
	std::string _ip_address;
	int _times;
	uint16_t _current_times;
};

int main(int argc, char* argv[]) {
	CX_UNUSED(argc);
	CX_UNUSED(argv);
	engine_type engine;
	auto s = std::make_shared<session>(engine);
	s->start_ping("192.168.1.1", 5);
	engine.run();
	return 0;
}