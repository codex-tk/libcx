#include <gtest/gtest.h>
#include <cx/cxdefine.hpp>
#include <cx/error.hpp>

TEST(error, t0) {
	std::error_code ec;
	ASSERT_FALSE(ec == cx::errc::success);

    ASSERT_FALSE(cx::make_error_code(cx::errc::success));

	ec = cx::make_error_code(cx::errc::success);
	ASSERT_FALSE(ec == std::errc::operation_canceled);

	ec = cx::make_error_code(cx::errc::operation_canceled);
	ASSERT_TRUE(ec == std::errc::operation_canceled);
	ASSERT_TRUE(std::errc::operation_canceled == ec);
}

// error_code 
template < typename Enum > 	struct is_check_sample : std::false_type {};

struct sample_wrapper {
	template < typename Enum , typename = typename std::enable_if_t<is_check_sample<Enum>::value> > 
	sample_wrapper( Enum  e ) {
		value = adl_function(e);
	}

	int value;
};

bool operator == ( const sample_wrapper& lhs , const sample_wrapper& rhs ) {
	return lhs.value == rhs.value;
}

namespace internal {

	enum sample {
		sample_0 , 
		sample_1 ,
	};

	int adl_function( sample s ) {
		return static_cast<int>(s);
	}

}

template <> struct is_check_sample<internal::sample> : std::true_type {};

TEST(adl,t0){
	sample_wrapper s(internal::sample_0);

	ASSERT_TRUE( internal::sample_0 == s );
	ASSERT_EQ( internal::sample_0 , s );
}