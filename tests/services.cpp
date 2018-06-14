#include <gtest/gtest.h>
#include <cx/cxdefine.hpp>
#include <cx/service_repository.hpp>

struct core_object {
};
template < typename ... Services >
class service : protected Services... {
public:
	service(core_object& core)
		: Services(core)...
	{}

	template < typename T > T& impl(void) {
		return *static_cast<T*>(this);
	}
private:
};
class empty_service {
public:
	empty_service(core_object&) {}
	int svcid(void) {
		return 0;
	}

private:
	int value;
};

class foo_service {
public:
	foo_service(core_object& ) {}
	int svcid(void) {
		return 1;
	}
private:
	int value;
};

TEST(service, t0) {
	core_object cobj;
	cx::service_repository< empty_service, foo_service > _s0(cobj);
	ASSERT_EQ(_s0.service( cx::tag<empty_service>()).svcid(), 0);
	ASSERT_EQ(_s0.service( cx::tag<foo_service>()).svcid(), 1);
}
#if CX_PLATFORM == CX_P_LINUX
#include <cx/io/internal/linux/epoll.hpp>
#include <cx/io/internal/reactor/reactor_socket_service.hpp>
#include <cx/io/basic_engine.hpp>
using svc = cx::io::ip::reactor_socket_service<
            cx::io::epoll , SOCK_STREAM, IPPROTO_TCP>;
TEST(epoll,to){
    cx::io::basic_engine< cx::io::epoll , svc > engine; 
    engine.service( cx::tag<svc>());
}
#endif

