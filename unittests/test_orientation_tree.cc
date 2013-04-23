
#include <gtest/gtest.h>
#include <node.hpp>

namespace {
TEST(Orientation, Resolve) {
  node node0(0);

  EXPECT_EQ((node*)0, node0.parent);
  EXPECT_EQ(0, node0.rank);
  EXPECT_EQ(1, node0.orient);

  node node1(1);
  EXPECT_EQ((node*)0, node1.parent);
  EXPECT_EQ(1, node1.orient);
  EXPECT_EQ(0, node1.rank);
  node* child1 = merge_nodes(node0, node1);
  ASSERT_NE((node*)0, child1->parent);
  node* root = child1->parent;
  EXPECT_EQ(1, child1->orient);
  EXPECT_EQ(0, child1->rank);
  EXPECT_EQ(1, root->rank);

  node node2(2);
  node2.orient = -1;
  node* child2 = merge_nodes(*root, node2);
  EXPECT_EQ(&node2, child2);
  EXPECT_EQ(root, child2->parent);
  EXPECT_EQ(-1, child2->orient);
  EXPECT_EQ(0, child2->rank);
  EXPECT_EQ(1, root->rank);

  root->orient = -1;
  EXPECT_EQ(-1, child1->resolve_orientation());
  EXPECT_EQ(1, child2->resolve_orientation());
}

TEST(Orientation, Depth) {
  node n0(0), n1(1), n2(2), n3(3);
  /*  Fix some orientations and some rank to obtain the given tree
       n2(-1)
      /     \
     n0(-1)  n3(-1)
    /
   n1(-1)
  */
  n1.orient = -1;
  n2.orient = -1;
  n0.rank   = 1;
  n2.rank   = 2;

  node* child0 = merge_nodes(n0, n1);
  EXPECT_EQ(&n1, child0);
  EXPECT_EQ(&n0, child0->parent);
  EXPECT_EQ(1, n0.rank);

  node* child1 = merge_nodes(n2, n3);
  EXPECT_EQ(&n3, child1);
  EXPECT_EQ(&n2, child1->parent);
  ASSERT_EQ(2, n2.rank);

  node* child2 = merge_nodes(n2, n0);
  EXPECT_EQ(&n0, child2);
  EXPECT_EQ(&n2, child2->parent);
  ASSERT_EQ(2, n2.rank);

  // First time query, path compression could take place
  ASSERT_EQ(-1, n1.resolve_orientation());
  ASSERT_EQ(1, n0.resolve_orientation());
  ASSERT_EQ(-1, n2.resolve_orientation());
  ASSERT_EQ(1, n3.resolve_orientation());

  ASSERT_EQ(-1, n2.orient);
  ASSERT_EQ((node*)0, n2.parent);
  ASSERT_EQ(-1, n0.orient);
  ASSERT_EQ(&n2, n0.parent);
  ASSERT_EQ(1, n1.orient);
  ASSERT_EQ(&n2, n1.parent);
  ASSERT_EQ(-1, n3.orient);
  ASSERT_EQ(&n2, n3.parent);

  // Second time query, no path compression, results should be
  // identical
  ASSERT_EQ(-1, n1.resolve_orientation());
  ASSERT_EQ(1, n0.resolve_orientation());
  ASSERT_EQ(-1, n2.resolve_orientation());
  ASSERT_EQ(1, n3.resolve_orientation());

}
}
