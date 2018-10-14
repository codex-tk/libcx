/**
 * @brief
 *
 * @file utils.cpp
 * @author ghtak
 * @date 2018-08-19
 */

#include <cx/base/utils.hpp>
#include <gtest/gtest.h>

void gprintf(const char *fmt, ...) {
    printf("[   USER   ] ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

TEST(cx_util, checksum) {
    int value = 32;
    ASSERT_EQ(cx::checksum(&value, sizeof(value)), 65503);

    char hello[] = "hello";
    ASSERT_EQ(cx::checksum(hello, strlen(hello)), 11708);
}

TEST(cx_util, clazz) {
    struct sample {
        int value;
        int value_4;
        void set(int i) { value = i; }
    };
    sample test_sample;
    test_sample.value = 0;

    cx::clazz<sample>::mem_ptr_t<int> int_ptr(&sample::value);
    (test_sample.*int_ptr) = 1;

    ASSERT_EQ(test_sample.value, 1);

    cx::clazz<sample>::mem_func_ptr_t<void(int)> func_ptr(&sample::set);
    (test_sample.*func_ptr)(2);

    ASSERT_EQ(test_sample.value, 2);

    ASSERT_EQ(cx::clazz<sample>::offset_of(&sample::value), 0);
    ASSERT_EQ(cx::clazz<sample>::offset_of(&sample::value_4), 4);

    ASSERT_EQ(
        cx::clazz<sample>::container_of(&test_sample.value, &sample::value),
        &test_sample);
    ASSERT_EQ(
        cx::clazz<sample>::container_of(&test_sample.value_4, &sample::value_4),
        &test_sample);
}

TEST(cx_util, null_type_size) {
    struct empty {};
    ASSERT_EQ(sizeof(empty), 1);
}

TEST(cx_util, hex_str) {
    int val[8] = {0x12345678, 0x34567812, 0x56781234, 0x78123456,
                  0x12345678, 0x34567812, 0x56781234, 0x78123456};

    auto str = cx::hex_str(val, sizeof(int) * 8);
    gprintf("%s", str.c_str());
}