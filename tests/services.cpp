#include <gtest/gtest.h>
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
	ASSERT_EQ(_s0.service<empty_service>().svcid(), 0);
	ASSERT_EQ(_s0.service<foo_service>().svcid(), 1);
}