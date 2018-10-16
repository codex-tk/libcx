#include "gtest/gtest.h"
#include <cx/base/basic_buf.hpp>

using namespace cx;

TEST(cx_base_basic_buf, t0) {
    shared_buf<char> null_buf;
    ASSERT_TRUE(null_buf.base() == nullptr);
    ASSERT_TRUE(null_buf.wr_ptr() == nullptr);
    ASSERT_TRUE(null_buf.rd_ptr() == nullptr);

    shared_buf<char> test_buf(10);
    ASSERT_EQ(test_buf.block().use_count(), 1);
    ASSERT_TRUE(test_buf.base() != nullptr);
    ASSERT_TRUE(test_buf.wr_ptr() != nullptr);
    ASSERT_TRUE(test_buf.rd_ptr() != nullptr);
    ASSERT_EQ(test_buf.base(), test_buf.wr_ptr());
    ASSERT_EQ(test_buf.wr_ptr(), test_buf.rd_ptr());

    do {
        shared_buf<char> buf_copy_ctor(test_buf);
        ASSERT_EQ(test_buf.block().use_count(), 2);
    } while (0);

    ASSERT_EQ(test_buf.block().use_count(), 1);

    // XXXXX-----
    ASSERT_EQ(test_buf.wr_ptr(5), 5);
    ASSERT_EQ(test_buf.size(), 5);
    ASSERT_EQ(test_buf.space(), 5);
    ASSERT_EQ(test_buf.wr_ptr(), test_buf.base() + 5);

    // 000XX-----
    ASSERT_EQ(test_buf.rd_ptr(3), 3);
    ASSERT_EQ(test_buf.size(), 2);
    ASSERT_EQ(test_buf.wr_ptr(), test_buf.base() + 5);
    ASSERT_EQ(test_buf.rd_ptr(), test_buf.base() + 3);

    // 000-------
    ASSERT_EQ(test_buf.wr_ptr(-5), -2);
    ASSERT_EQ(test_buf.size(), 0);

    // ----------
    ASSERT_EQ(test_buf.rd_ptr(-1000), -3);
    ASSERT_EQ(test_buf.wr_ptr(-5), -3);
    ASSERT_EQ(test_buf.size(), 0);
    ASSERT_EQ(test_buf.space(), 10);

    do {
        shared_buf<char> buf_move_ctor(std::move(test_buf));
        ASSERT_TRUE(test_buf.size() == 0);
        ASSERT_TRUE(test_buf.space() == 0);
    } while (0);
}

TEST(cx_base_basic_buf, ctor) {
    shared_buf<char> buf(1024);
    ASSERT_EQ(buf.capacity(), 1024);
    ASSERT_EQ(buf.space(), 1024);
    ASSERT_EQ(buf.size(), 0);
}

TEST(cx_base_basic_buf, skip) {
    shared_buf<char> buf(1024);
    ASSERT_EQ(buf.rd_ptr(24), 0);
    ASSERT_EQ(buf.wr_ptr(24), 24);

    ASSERT_EQ(buf.space(), 1024 - 24);
    ASSERT_EQ(buf.size(), 24);

    ASSERT_EQ(buf.rd_ptr(4), 4);
    ASSERT_EQ(buf.size(), 20);

    ASSERT_EQ(buf.rd_ptr(-2), -2);
    ASSERT_EQ(buf.size(), 22);

    ASSERT_EQ(buf.rd_ptr(-10), -2);
    ASSERT_EQ(buf.size(), 24);

    ASSERT_EQ(buf.wr_ptr(-2), -2);
    ASSERT_EQ(buf.size(), 22);

    ASSERT_EQ(buf.wr_ptr(10000), 1002);
    ASSERT_EQ(buf.size(), 1024);

    ASSERT_EQ(buf.wr_ptr(-10000), -1024);
    ASSERT_EQ(buf.size(), 0);

    ASSERT_EQ(buf.wr_ptr(24), 24);
    ASSERT_EQ(buf.rd_ptr(4), 4);

    ASSERT_EQ(buf.size(), 20);

    ASSERT_EQ(buf.wr_ptr(-24), -20);
}

TEST(cx_base_basic_buf, ptr) {
    shared_buf<char> buf(1024);
    ASSERT_EQ(buf.rd_ptr(), buf.wr_ptr());
    int i = 0;
    while (buf.space() >= static_cast<int>(sizeof(int))) {
        memcpy(buf.wr_ptr(), &i, sizeof(int));
        buf.wr_ptr(sizeof(int));
        ++i;
    }
    ASSERT_EQ(buf.space(), 0);
    i = 0;
    ASSERT_NE(buf.size(), 0);
    while (buf.size() >= static_cast<int>(sizeof(int))) {
        int value = 0;
        memcpy(&value, buf.rd_ptr(), sizeof(int));
        buf.rd_ptr(sizeof(int));
        ASSERT_EQ(i, value);
        ++i;
    }
}

TEST(cx_base_basic_buf, owner) {
    shared_buf<char> buf(1024);
    ASSERT_EQ(buf.block().use_count(), 1);
    {
        shared_buf<char> shared_buf(buf);
        ASSERT_EQ(buf.block().use_count(), 2);
    }
    ASSERT_EQ(buf.block().use_count(), 1);

    shared_buf<char> move(std::move(buf));
    ASSERT_EQ(move.block().use_count(), 1);
}

TEST(cx_base_basic_buf, owner1) {
    shared_buf<char> buf(1024);
    ASSERT_EQ(buf.block().use_count(), 1);
    {
        shared_buf<char> shared_buf(0);
        shared_buf = buf;
        ASSERT_EQ(buf.block().use_count(), 2);
    }
    ASSERT_EQ(buf.block().use_count(), 1);

    shared_buf<char> move(0);
    move = std::move(buf);
    ASSERT_EQ(move.block().use_count(), 1);
}

TEST(cx_base_basic_buf, reserve) {
    shared_buf<char> buf(4);
    void *orig_ptr = buf.base();
    // 01--
    buf.wr_ptr(2);
    ASSERT_EQ(buf.base(), buf.rd_ptr());
    //-1--
    buf.rd_ptr(1);
    ASSERT_NE(buf.base(), buf.rd_ptr());
    buf.reserve(3);
    // 1---
    ASSERT_EQ(buf.base(), orig_ptr);
    ASSERT_EQ(buf.space(), 3);
    ASSERT_EQ(buf.base(), buf.rd_ptr());
}
/*
TEST(basic_buffer, consume_commit_prepare) {
        cx::basic_buffer<char> buf(4);
        memset(buf.prepare(1), 0x01, 1);
        ASSERT_EQ(buf.commit(1), 1);
        ASSERT_EQ(buf.size(), 1);
        ASSERT_EQ(buf.space(), 3);
        ASSERT_EQ(buf.consume(1), 1);
}*/

TEST(cx_base_basic_buf, fixed_buf) {
    fixed_buf<char> buf;
    char raw_buf[1024];
    buf = std::move(fixed_buf<char>(raw_buf, 1024));

    fixed_buf<char> buf_with_deleter(new char[1024], 1024,
                               [](char *ptr) { delete[] ptr; });


}