/**
 * @brief 
 * 
 * @file utils.cpp
 * @author ghtak
 * @date 2018-08-19
 */

#include <gtest/gtest.h>
#include <cx/base/utils.hpp>

TEST(cx_util, checksum){
	int value = 32;
	ASSERT_EQ(cx::checksum(&value, sizeof(value)), 65503);
	
	char hello[] = "hello";
	ASSERT_EQ(cx::checksum(hello, strlen(hello)),11708);
}

TEST(cx_util, clazz) {
	struct sample{ 
		int value; 
		int value_4;
		void set(int i) { value = i; }
	};
	sample test_sample;
	test_sample.value = 0;

	cx::clazz<sample>::mem_ptr_t<int> int_ptr( &sample::value );
	(test_sample.*int_ptr) = 1;

	ASSERT_EQ(test_sample.value, 1);

	cx::clazz<sample>::mem_func_ptr_t<void(int)> func_ptr(&sample::set);
	(test_sample.*func_ptr)(2);

	ASSERT_EQ(test_sample.value, 2);
	
	ASSERT_EQ(cx::clazz<sample>::offset_of(&sample::value), 0);
	ASSERT_EQ(cx::clazz<sample>::offset_of(&sample::value_4), 4);

	ASSERT_EQ(cx::clazz<sample>::container_of(&test_sample.value, &sample::value), &test_sample);
	ASSERT_EQ(cx::clazz<sample>::container_of(&test_sample.value_4, &sample::value_4), &test_sample);

}