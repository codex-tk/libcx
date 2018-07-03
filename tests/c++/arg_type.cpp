#include <gtest/gtest.h>
#include <cx/log/core.hpp>

namespace keyword {

    struct name{
		name(const char* p) : value(p) {}
		const char* value;
	};

    struct addr{
		addr(const char* p) : value(p) {}
		const char* value;
	};

	struct _name_gen {
		name operator=(const char* p) { return name(p); }
	};

	_name_gen name_gen;
}

class ArgToStream{
public:
    template < typename ... Ts >
    ArgToStream( Ts&& ... ts) noexcept {
        using expend = int[];
        (void)expend{0, (ProcessArg(ts), 0)...};
    }

    void ProcessArg( const keyword::name& name ){
        CXLOG_D( "ArgToStream" , "Name %s" , name.value );
    }

    void ProcessArg( const keyword::addr& addr ){
        CXLOG_D( "ArgToStream" , "Addr %s", addr.value);
    }
};

TEST( arg , named_param ) {
    ArgToStream ats(
		keyword::name("My Name Is ") ,
		keyword::addr("My Address Is ")
	);

	ArgToStream ats0(
		keyword::addr("My Address Is ") ,
		keyword::name("My Name Is ")
	);

	ArgToStream ats2(
		keyword::name_gen = "Generated Name " ,
		keyword::addr("My Address Is "),
		keyword::name("My Name Is ")
	);
}