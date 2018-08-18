#include <gtest/gtest.h>
#include "tests/gprintf.hpp"
#include <list>

namespace {
template < typename Type >
struct node {
    node( void ){

    }
    Type value;
    node* left;
    node* right;
};

template < typename Type >
void pre_order( node<Type>* pnode , std::vector<Type>& outputs ) {
    if ( pnode == nullptr ) return;
    outputs.push_back(pnode->value);
    pre_order( pnode->left , outputs );
    pre_order( pnode->right , outputs );
}

template < typename Type >
void pre_order_i( node<Type>* pnode , std::vector<Type>& outputs ) {
    if ( pnode == nullptr ) return;
    std::vector< node<Type>* > stack { pnode };
    while ( !stack.empty() ) {
        node<Type>* pn = stack.back();
        stack.pop_back();
        outputs.push_back(pn->value);
        if ( pn->right ) stack.push_back(pn->right);
        if ( pn->left ) stack.push_back(pn->left);
    }
}

template < typename Type >
void in_order( node<Type>* pnode , std::vector<Type>& outputs ) {
    if ( pnode == nullptr ) return;
    in_order( pnode->left , outputs );
    outputs.push_back(pnode->value);
    in_order( pnode->right , outputs );
}

template < typename Type >
void in_order_i( node<Type>* pnode , std::vector<Type>& outputs ) {
    if ( pnode == nullptr ) return;
    std::vector< node<Type>* > stack;
    while ( pnode || !stack.empty() ) {
        while ( pnode ) {
            stack.push_back(pnode);
            pnode = pnode->left;
        }
        pnode = stack.back(); stack.pop_back();
        outputs.push_back(pnode->value);
        pnode = pnode->right;
    }
}

template < typename Type >
void post_order( node<Type>* pnode , std::vector<Type>& outputs ) {
    if ( pnode == nullptr ) return;
    post_order( pnode->left , outputs );
    post_order( pnode->right , outputs );
    outputs.push_back(pnode->value);
}

template < typename Type >
void post_order_i( node<Type>* pnode , std::vector<Type>& outputs ) {
    if ( pnode == nullptr ) return;
    std::vector< node<Type>* > stack;

    while ( pnode || !stack.empty() ) {
        while( pnode ) {
            if (pnode->right)
                stack.push_back(pnode->right);
            stack.push_back(pnode);
            pnode = pnode->left;
        }
        pnode = stack.back();
        stack.pop_back();
        if ( pnode->right 
            && !stack.empty() 
            && stack.back() == pnode->right )
        {
            stack.pop_back();
            stack.push_back(pnode);
            pnode = pnode->right;
        } else {
            outputs.push_back(pnode->value);
            pnode = nullptr;
        }
    }
}

template < typename Type >
void bfs0( node<Type>* pnode , std::vector<Type>& outputs ) {
    std::list< node<Type> * > q { pnode };
    while( !q.empty() ){
        node<Type>* n = q.front();
        q.pop_front();
        outputs.push_back(n->value);
        if ( n->left ) q.push_back(n->left);
        if ( n->right ) q.push_back(n->right);
    }
}
}

class tree_retrieve : public testing::Test {
public:
    void SetUp() override {
        for ( int i = 0 ; i < 8 ; ++i ){
            node<char> n; 
            n.value = static_cast<char>(i + '0');
            n.left = nullptr;
            n.right = nullptr;
            nodes.push_back(n);
        }
        for ( std::size_t i = 0 ; i < 8 ; ++i ){
            std::size_t child_idx = i * 2 + 1;
            if ( nodes.size() > child_idx ) {
                nodes[i].left = &nodes[child_idx];
                ++child_idx;
                if ( nodes.size() > child_idx ) {
                    nodes[i].right = &nodes[child_idx];
                }
            }
        }
    }
    void TearDown() override {
    }
protected:
    std::vector< node<char> > nodes;
};

/*
      0
    1    2
  3  4  5  6
7
*/

TEST_F( tree_retrieve , order ) {
    std::vector< char > pre_order_o;
    pre_order( &nodes[0] , pre_order_o );
    pre_order_o.push_back(0);


    std::vector< char > pre_order_o_i;
    pre_order_i( &nodes[0] , pre_order_o_i );
    pre_order_o_i.push_back(0);

    ASSERT_STREQ( &pre_order_o[0] , &pre_order_o_i[0] );

    std::vector< char > in_order_o;
    in_order( &nodes[0] , in_order_o );
    in_order_o.push_back(0);

    std::vector< char > in_order_o_i;
    in_order_i( &nodes[0] , in_order_o_i );
    in_order_o_i.push_back(0);

    ASSERT_STREQ( &in_order_o[0] , &in_order_o_i[0] );

    std::vector< char > post_order_o;
    post_order( &nodes[0] , post_order_o );
    post_order_o.push_back(0);

    std::vector< char > post_order_o_i;
    post_order_i( &nodes[0] , post_order_o_i );
    post_order_o_i.push_back(0);

    std::vector< char > bfs_o;
    bfs0( &nodes[0],bfs_o);
    bfs_o.push_back(0);

    gprintf( "%s:%s\n" 
         , &pre_order_o[0] , &pre_order_o_i[0] );
	gprintf( "%s:%s\n"
         , &in_order_o[0] , &in_order_o_i[0] );
	gprintf( "%s:%s\n"
         , &post_order_o[0]
         , &post_order_o_i[0] );
}