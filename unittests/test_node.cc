#include <gtest/gtest.h>
#include <node.hpp>
#include <edge_list.hpp>

namespace {
TEST(Node, Simple) {
  node n(0);

  EXPECT_EQ(0, n->id);
  EXPECT_EQ(0, n->int_good);
  EXPECT_EQ(0, n->int_bad);
  EXPECT_EQ(1, n->size);
  EXPECT_EQ(1, n->orient);
}

  TEST(Node, edgefunctions) {

    //Make 2 nodes and an edge. So we can put together
    node n1(0),n2(1);
    edge_base<node> e1(&n1,&n2,2,1);
    
    EXPECT_EQ(0, n1->id);
    EXPECT_EQ(1, n2->id);

    //Primary test of far id. 
    n1.add_edge(&e1);   
    EXPECT_EQ((size_t)1, n1->edges.size() );
    EXPECT_EQ(1, n1.far_id(&e1));

    //Test that we are actually getting the right ID out.
    n2.add_edge(&e1);
    EXPECT_EQ((size_t)1,n2->edges.size() );
    EXPECT_EQ(0, n2.far_id(&e1));
  }
    
    
    
}
