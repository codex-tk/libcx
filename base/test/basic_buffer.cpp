#include "gtest/gtest.h"
#include <cx/base/basic_buffer.hpp>

TEST(cx_base_basic_buffer, t0) {
	cx::basic_buffer<char> null_buf;
	ASSERT_TRUE(null_buf.block() == nullptr);
	ASSERT_TRUE(null_buf.base() == nullptr);
	ASSERT_TRUE(null_buf.wrptr() == nullptr);
	ASSERT_TRUE(null_buf.rdptr() == nullptr);

	cx::basic_buffer<char> test_buf(10);
	ASSERT_TRUE(test_buf.block() != nullptr);
	ASSERT_EQ(test_buf.block()->use_count(), 1);
	ASSERT_TRUE(test_buf.base() != nullptr);
	ASSERT_TRUE(test_buf.wrptr() != nullptr);
	ASSERT_TRUE(test_buf.rdptr() != nullptr);
	ASSERT_EQ(test_buf.base(), test_buf.wrptr());
	ASSERT_EQ(test_buf.wrptr(), test_buf.rdptr());

	do {
		cx::basic_buffer<char> buf_copy_ctor(test_buf);
		ASSERT_EQ(test_buf.block()->use_count(), 2);
	} while (0);

	ASSERT_EQ(test_buf.block()->use_count(), 1);

	// XXXXX-----
	ASSERT_EQ(test_buf.wrptr(5), 5);
	ASSERT_EQ(test_buf.rdsize(), 5);
	ASSERT_EQ(test_buf.wrsize(), 5);
	ASSERT_EQ(test_buf.wrptr(), test_buf.base() + 5);

	// 000XX-----
	ASSERT_EQ(test_buf.rdptr(3), 3);
	ASSERT_EQ(test_buf.rdsize(), 2);
	ASSERT_EQ(test_buf.wrptr(), test_buf.base() + 5);
	ASSERT_EQ(test_buf.rdptr(), test_buf.base() + 3);

	// 000-------
	ASSERT_EQ(test_buf.wrptr(-5), -2);
	ASSERT_EQ(test_buf.rdsize(), 0);

	// ----------
	ASSERT_EQ(test_buf.rdptr(-1000), -3);
	ASSERT_EQ(test_buf.wrptr(-5), -3);
	ASSERT_EQ(test_buf.rdsize(), 0);
	ASSERT_EQ(test_buf.wrsize(), 10);

	do {
		cx::basic_buffer<char> buf_move_ctor(std::move(test_buf));
		ASSERT_TRUE(test_buf.block() == nullptr);
		ASSERT_TRUE(test_buf.rdsize() == 0);
		ASSERT_TRUE(test_buf.wrsize() == 0);
	} while (0);
}

TEST(cx_base_basic_buffer, ctor) {

	cx::basic_buffer<char> buf(1024);
	ASSERT_EQ(buf.size(), 1024);
	ASSERT_EQ(buf.wrsize(), 1024);
	ASSERT_EQ(buf.rdsize(), 0);
}

TEST(cx_base_basic_buffer, skip) {
	cx::basic_buffer<char> buf(1024);
	ASSERT_EQ(buf.rdptr(24), 0);
	ASSERT_EQ(buf.wrptr(24), 24);

	ASSERT_EQ(buf.wrsize(), 1024 - 24);
	ASSERT_EQ(buf.rdsize(), 24);

	ASSERT_EQ(buf.rdptr(4), 4);
	ASSERT_EQ(buf.rdsize(), 20);

	ASSERT_EQ(buf.rdptr(-2), -2);
	ASSERT_EQ(buf.rdsize(), 22);

	ASSERT_EQ(buf.rdptr(-10), -2);
	ASSERT_EQ(buf.rdsize(), 24);

	ASSERT_EQ(buf.wrptr(-2), -2);
	ASSERT_EQ(buf.rdsize(), 22);

	ASSERT_EQ(buf.wrptr(10000), 1002);
	ASSERT_EQ(buf.rdsize(), 1024);

	ASSERT_EQ(buf.wrptr(-10000), -1024);
	ASSERT_EQ(buf.rdsize(), 0);

	ASSERT_EQ(buf.wrptr(24), 24);
	ASSERT_EQ(buf.rdptr(4), 4);

	ASSERT_EQ(buf.rdsize(), 20);

	ASSERT_EQ(buf.wrptr(-24), -20);
}

TEST(cx_base_basic_buffer, ptr) {
	cx::basic_buffer<char> buf(1024);
	ASSERT_EQ(buf.rdptr(), buf.wrptr());
	int i = 0;
	while (buf.wrsize() >= static_cast<int>(sizeof(int))) {
		memcpy(buf.wrptr(), &i, sizeof(int));
		buf.wrptr(sizeof(int));
		++i;
	}
	ASSERT_EQ(buf.wrsize(), 0);
	i = 0;
	ASSERT_NE(buf.rdsize(), 0);
	while (buf.rdsize() >= static_cast<int>(sizeof(int))) {
		int value = 0;
		memcpy(&value, buf.rdptr(), sizeof(int));
		buf.rdptr(sizeof(int));
		ASSERT_EQ(i, value);
		++i;
	}
}


TEST(cx_base_basic_buffer, owner) {
	cx::basic_buffer<char> buf(1024);
	ASSERT_EQ(buf.block()->use_count(), 1);
	{
		cx::basic_buffer<char> shared_buf(buf);
		ASSERT_EQ(buf.block()->use_count(), 2);
	}
	ASSERT_EQ(buf.block()->use_count(), 1);

	cx::basic_buffer<char> move(std::move(buf));
	ASSERT_EQ(move.block()->use_count(), 1);
}

TEST(cx_base_basic_buffer, owner1) {
	cx::basic_buffer<char> buf(1024);
	ASSERT_EQ(buf.block()->use_count(), 1);
	{
		cx::basic_buffer<char> shared_buf(0);
		shared_buf = buf;
		ASSERT_EQ(buf.block()->use_count(), 2);
	}
	ASSERT_EQ(buf.block()->use_count(), 1);

	cx::basic_buffer<char> move(0);
	move = std::move(buf);
	ASSERT_EQ(move.block()->use_count(), 1);
}

TEST(cx_base_basic_buffer, reserve) {
	cx::basic_buffer<char> buf(4);
	void* orig_ptr = buf.base();
	// 01--
	buf.wrptr(2);
	ASSERT_EQ(buf.base(), buf.rdptr());
	//-1--
	buf.rdptr(1);
	ASSERT_NE(buf.base(), buf.rdptr());
	buf.reserve(3);
	// 1---
	ASSERT_EQ(buf.base(), orig_ptr);
	ASSERT_EQ(buf.wrsize(), 3);
	ASSERT_EQ(buf.base(), buf.rdptr());
}
/*
TEST(basic_buffer, consume_commit_prepare) {
	cx::basic_buffer<char> buf(4);
	memset(buf.prepare(1), 0x01, 1);
	ASSERT_EQ(buf.commit(1), 1);
	ASSERT_EQ(buf.rdsize(), 1);
	ASSERT_EQ(buf.wrsize(), 3);
	ASSERT_EQ(buf.consume(1), 1);
}*/