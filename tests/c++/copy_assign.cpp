#include <gtest/gtest.h>
#include <functional>

namespace {

	static int basic_copy_assign_call_ids[7] = { 0 , };

	class basic_copy_assign {
	public:
		basic_copy_assign(void) { basic_copy_assign_call_ids[0] += 1; }
		basic_copy_assign(const basic_copy_assign&) { basic_copy_assign_call_ids[1] += 1; }
		basic_copy_assign(basic_copy_assign&&) { basic_copy_assign_call_ids[2] += 1; }
		template < typename HandlerType > basic_copy_assign(HandlerType&&) { basic_copy_assign_call_ids[3] += 1; }

		basic_copy_assign& operator=(const basic_copy_assign&) {
			basic_copy_assign_call_ids[4] += 1;
			return *this;
		}

		basic_copy_assign& operator=(basic_copy_assign&&) {
			basic_copy_assign_call_ids[5] += 1;
			return *this;
		}

		template < typename HandlerType >
		basic_copy_assign& operator=(HandlerType&&) {
			basic_copy_assign_call_ids[6] += 1;
			return *this;
		}

		void operator()(void) { }
	};

	static int explicit_copy_assign_call_ids[7] = { 0 , };
	template <typename T> struct is_self_type : std::false_type {};
	class explicit_copy_assign {
	public:
		explicit_copy_assign(void) { explicit_copy_assign_call_ids[0] += 1; }
		explicit_copy_assign(const explicit_copy_assign&) { explicit_copy_assign_call_ids[1] += 1; }
		explicit_copy_assign(explicit_copy_assign&&) { explicit_copy_assign_call_ids[2] += 1; }
		template < typename HandlerType, typename = std::enable_if_t< !is_self_type< std::remove_reference_t<HandlerType>>::value >>
		explicit_copy_assign(HandlerType&&) { explicit_copy_assign_call_ids[3] += 1; }

		explicit_copy_assign& operator=(const explicit_copy_assign&) {
			explicit_copy_assign_call_ids[4] += 1;
			return *this;
		}

		explicit_copy_assign& operator=(explicit_copy_assign&&) {
			explicit_copy_assign_call_ids[5] += 1;
			return *this;
		}

		template < typename HandlerType, typename = std::enable_if_t< !is_self_type< std::remove_reference_t<HandlerType>>::value >>
		explicit_copy_assign& operator=(HandlerType&&) {
			explicit_copy_assign_call_ids[6] += 1;
			return *this;
		}

		void operator()(void) { }
	};
	template <> struct is_self_type<explicit_copy_assign> : std::true_type {};


	static int explicit_copy_assign0_call_ids[7] = { 0 , };
	class explicit_copy_assign0 {
	public:
		explicit_copy_assign0(void) { explicit_copy_assign0_call_ids[0] += 1; }
		explicit_copy_assign0(const explicit_copy_assign0&) { explicit_copy_assign0_call_ids[1] += 1; }
		explicit_copy_assign0(explicit_copy_assign0&&) { explicit_copy_assign0_call_ids[2] += 1; }
		template < typename HandlerType
			, typename = typename std::enable_if< !std::is_same_v< explicit_copy_assign0, std::remove_reference_t<HandlerType>>>::type>
		explicit_copy_assign0(HandlerType&&) { explicit_copy_assign0_call_ids[3] += 1; }

		explicit_copy_assign0& operator=(const explicit_copy_assign0&) {
			explicit_copy_assign0_call_ids[4] += 1;
			return *this;
		}

		explicit_copy_assign0& operator=(explicit_copy_assign0&&) {
			explicit_copy_assign0_call_ids[5] += 1;
			return *this;
		}

		template < typename HandlerType
			, typename = typename std::enable_if< !std::is_same_v< explicit_copy_assign0, std::remove_reference_t<HandlerType>>>::type>
		explicit_copy_assign0& operator=(HandlerType&&) {
			explicit_copy_assign0_call_ids[6] += 1;
			return *this;
		}

		void operator()(void) { }
	};
}

TEST(copy_assign, basic_test ) {
	basic_copy_assign object;
	ASSERT_EQ(basic_copy_assign_call_ids[0], 1);

	const basic_copy_assign cobject;
	ASSERT_EQ(basic_copy_assign_call_ids[0], 2);
	
	basic_copy_assign cp0(cobject);
	ASSERT_EQ(basic_copy_assign_call_ids[1], 1);

	basic_copy_assign cp1(object);
	//expect (basic_copy_assign_call_ids[1], 1);
	//but actual
	ASSERT_EQ(basic_copy_assign_call_ids[3], 1);

	basic_copy_assign mv(std::move(cp0));
	ASSERT_EQ(basic_copy_assign_call_ids[2], 1);

	basic_copy_assign h([]() {});
	ASSERT_EQ(basic_copy_assign_call_ids[3], 2);

	object = cobject;
	ASSERT_EQ(basic_copy_assign_call_ids[4], 1);

	object = cp0;
	//ASSERT_EQ(basic_copy_assign_call_ids[4], 2);
	ASSERT_EQ(basic_copy_assign_call_ids[6], 1);

	object = std::move(cp0);
	ASSERT_EQ(basic_copy_assign_call_ids[5], 1);

	object = []() {};
	ASSERT_EQ(basic_copy_assign_call_ids[6], 2);
}


TEST(copy_assign, explicit_test) {
	explicit_copy_assign object;
	ASSERT_EQ(explicit_copy_assign_call_ids[0], 1);

	const explicit_copy_assign cobject;
	ASSERT_EQ(explicit_copy_assign_call_ids[0], 2);

	explicit_copy_assign cp0(cobject);
	ASSERT_EQ(explicit_copy_assign_call_ids[1], 1);

	explicit_copy_assign cp1(object);
	ASSERT_EQ(explicit_copy_assign_call_ids[1], 2);

	explicit_copy_assign mv(std::move(cp0));
	ASSERT_EQ(explicit_copy_assign_call_ids[2], 1);

	explicit_copy_assign h([]() {});
	ASSERT_EQ(explicit_copy_assign_call_ids[3], 1);

	object = cobject;
	ASSERT_EQ(explicit_copy_assign_call_ids[4], 1);

	object = cp0;
	ASSERT_EQ(explicit_copy_assign_call_ids[4], 2);

	object = std::move(cp0);
	ASSERT_EQ(explicit_copy_assign_call_ids[5], 1);

	object = []() {};
	ASSERT_EQ(explicit_copy_assign_call_ids[6], 1);
}

TEST(copy_assign, explicit0_test) {
	explicit_copy_assign0 object;
	ASSERT_EQ(explicit_copy_assign0_call_ids[0], 1);

	const explicit_copy_assign0 cobject;
	ASSERT_EQ(explicit_copy_assign0_call_ids[0], 2);

	explicit_copy_assign0 cp0(cobject);
	ASSERT_EQ(explicit_copy_assign0_call_ids[1], 1);

	explicit_copy_assign0 cp1(object);
	ASSERT_EQ(explicit_copy_assign0_call_ids[1], 2);

	explicit_copy_assign0 mv(std::move(cp0));
	ASSERT_EQ(explicit_copy_assign0_call_ids[2], 1);

	explicit_copy_assign0 h([]() {});
	ASSERT_EQ(explicit_copy_assign0_call_ids[3], 1);

	object = cobject;
	ASSERT_EQ(explicit_copy_assign0_call_ids[4], 1);

	object = cp0;
	ASSERT_EQ(explicit_copy_assign0_call_ids[4], 2);

	object = std::move(cp0);
	ASSERT_EQ(explicit_copy_assign0_call_ids[5], 1);

	object = []() {};
	ASSERT_EQ(explicit_copy_assign0_call_ids[6], 1);
}