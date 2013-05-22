#include <gtest/gtest.h>
#include <horient.hpp>

namespace {
class Node : public ::testing::Test {
public:
  Node() : n1(0), n2(1) { }

protected:
  typedef edge_base<node> edge;
  node n1, n2;
  master_list_type master_edge;
  edge_ptr edge_iterator;

  virtual void SetUp() {
    //Make 2 nodes and an edge. So we can put together
    master_edge.insert(edge(n1, n2, 2, 1, 5, 3));
  }
};

  TEST_F(Node, Simple) {
    EXPECT_EQ(0, n1.id);
    EXPECT_EQ(0, n1.int_good);
    EXPECT_EQ(0, n1.int_bad);
    EXPECT_EQ(1, n1.size);
    EXPECT_EQ(1, n1.orient);

    EXPECT_EQ(1, n2.id);
    EXPECT_EQ(0, n2.int_good);
    EXPECT_EQ(0, n2.int_bad);
    EXPECT_EQ(1, n2.size);
    EXPECT_EQ(1, n2.orient);
  }

  TEST_F(Node, created_2){
    EXPECT_EQ(0, n1.id);
    EXPECT_EQ(1, n2.id);
  }

  TEST_F(Node, add_edge) {
    edge_iterator=master_edge.begin();
    n1.add_edge(edge_iterator);
    EXPECT_EQ((size_t)1, n1.edges.local_list.size() );
  }

  TEST_F(Node, far_id) {
    //Primary test of far id.
    edge_iterator=master_edge.begin();
    n1.add_edge(edge_iterator);
    EXPECT_EQ((size_t)1, n1.edges.local_list.size() );
    EXPECT_EQ(1, n1.far_id(edge_iterator));

    //Test that we are actually getting the right ID out.
    n2.add_edge(edge_iterator);
    EXPECT_EQ((size_t)1,n2.edges.local_list.size() );
    EXPECT_EQ(0, n2.far_id(edge_iterator));
  }


  TEST_F(Node, flip_node) {
    //Primary test of far id.
    edge_iterator=master_edge.begin();
    n1.add_edge(edge_iterator);

    //Check IDs
    EXPECT_EQ( 0, edge_iterator->n1.id);
    EXPECT_EQ( 1, edge_iterator->n2.id);

    //Check Orients
    EXPECT_EQ( 1, edge_iterator->n1.orient);
    EXPECT_EQ( 1, edge_iterator->n2.orient);

    //Check good/bad
    EXPECT_EQ( 2, edge_iterator->good );
    EXPECT_EQ( 1, edge_iterator->bad  );
    EXPECT_EQ( 5, edge_iterator->good2);
    EXPECT_EQ( 3, edge_iterator->bad2 );

    //Try to flip node 1
    n1.flip_node();

    //Orient should be different, and good/bad on edge swapped.
    EXPECT_EQ( -1, edge_iterator->n1.orient);
    EXPECT_EQ( 1, edge_iterator->good );
    EXPECT_EQ( 2, edge_iterator->bad  );
    EXPECT_EQ( 3, edge_iterator->good2);
    EXPECT_EQ( 5, edge_iterator->bad2 );

    //Now flip node 2.
    n2.flip_node();

    //Note these are all NOT equal. Because we
    // have not added the edge TO node 2... so it doesn't flip it.

    //Could change orient on node 2,
    // and return good/bad on edge to original values.
    EXPECT_EQ( -1, edge_iterator->n2.orient);
    EXPECT_NE( 2, edge_iterator->good );
    EXPECT_NE( 1, edge_iterator->bad  );
    EXPECT_NE( 5, edge_iterator->good2);
    EXPECT_NE( 3, edge_iterator->bad2 );

    //If we add the edge to the node we can flip it!
    n2.add_edge(edge_iterator);
    n2.flip_node();

    //Should change orient on node 2, (always, this time to original)
    // and return good/bad on edge to original values. (because it's added)
    EXPECT_EQ( 1, edge_iterator->n2.orient);
    EXPECT_EQ( 2, edge_iterator->good );
    EXPECT_EQ( 1, edge_iterator->bad  );
    EXPECT_EQ( 5, edge_iterator->good2);
    EXPECT_EQ( 3, edge_iterator->bad2 );

 }
}
