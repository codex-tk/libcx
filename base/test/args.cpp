/**
 * @brief
 *
 * @file args.cpp
 * @author ghtak
 * @date 2018-08-24
 */
#include <cx/base/basic_arg.hpp>
#include <gtest/gtest.h>

namespace keywords {
using name = cx::basic_arg<std::string>;
using name_generator_type = cx::basic_arg_generator<name>;

name_generator_type name_generator;

using age = cx::basic_arg<int>;
using age_generator_type = cx::basic_arg_generator<age>;

age_generator_type age_generator;

#ifdef SAME_TYPE_TEST
#error "Do Not USE SameType"
using same_type_0 = int;
using same_type_1 = int;
using same_type_0_t = cx::basic_arg<same_type_0>;
using same_type_1_t = cx::basic_arg<same_type_1>;
#endif
CX_DECLARE_ARG(name1, std::string);
} // namespace keywords

struct option {
    template <typename... Ts> option(Ts &&... ts) { CX_EXPAND_ARGS(set, ts); }

    void set(const keywords::name &arg) { this->name = arg.value; }
    void set(const keywords::age &arg) { this->age = arg.value; }
    void set(const keywords::name1 &arg) { this->name1 = arg.value; }

    std::string name;
    std::string name1;
    int age;

#ifdef SAME_TYPE_TEST
    void set(const keywords::same_type_0_t &arg) { this->age = arg.value; }
    void set(const keywords::same_type_1_t &arg) { this->age = arg.value; }
    keywords::same_type_0 s0;
    keywords::same_type_1 s1;
#endif
};

TEST(args, basic) {
    option opt(keywords::name("name"), keywords::age(1),
               keywords::name1("name1"));

    ASSERT_TRUE(opt.name == "name");
    ASSERT_TRUE(opt.name1 == "name1");
    ASSERT_TRUE(opt.age == 1);

    option opt2(keywords::age_generator = 32,
                keywords::name_generator = "name_gen",
                keywords::name1_generator = "name1_gen");

    ASSERT_TRUE(opt2.name == "name_gen");
    ASSERT_TRUE(opt2.name1 == "name1_gen");
    ASSERT_TRUE(opt2.age == 32);

    option opt3(keywords::age_generator(32),
                keywords::name_generator("name_gen"),
                keywords::name1_generator("name1_gen"));

    ASSERT_TRUE(opt2.name == "name_gen");
    ASSERT_TRUE(opt2.name1 == "name1_gen");
    ASSERT_TRUE(opt2.age == 32);
}

#ifdef SAME_TYPE_TEST
TEST(args, same_type) {
    option opt(keywords::name("name"), keywords::age(1),
               keywords::same_type_0_t(0), keywords::same_type_1_t(1));

    ASSERT_TRUE(opt.name == "name");
    ASSERT_TRUE(opt.age == 1);
    ASSERT_TRUE(opt.s0 == 0);
    ASSERT_TRUE(opt.s1 == 1);
}
#endif