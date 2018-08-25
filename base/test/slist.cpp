/**
 * @brief 
 * 
 * @file slist.cpp
 * @author ghtak
 * @date 2018-08-25
 */
#include <gtest/gtest.h>
#include <cx/base/slist.hpp>

namespace {
	class node_impl 
		: public cx::slist<node_impl>::node {
	};
}

TEST(cx_base_slist,basic){
	cx::slist<node_impl> list;
	node_impl impl;
	list.add_tail(&impl);
	ASSERT_TRUE(!list.empty());
	ASSERT_EQ(list.remove_head() , &impl);
}