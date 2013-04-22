
#include <gtest/gtest.h>
#include <node.hpp>

namespace {
TEST(Orientation, Resolve) {
  node root(0);

  EXPECT_EQ((node*)0, root.parent);
  EXPECT_EQ(1, root.orient);

  node child1(1);
  EXPECT_EQ((node*)0, child1.parent);
  EXPECT_EQ(1, child1.orient);
  child1.attach_to(&root);
  EXPECT_EQ(&root, child1.parent);
  EXPECT_EQ(1, child1.orient);

  node child2(2);
  child2.orient = -1;
  child2.attach_to(&root);
  EXPECT_EQ(&root, child2.parent);
  EXPECT_EQ(-1, child2.orient);

  root.orient = -1;
  EXPECT_EQ(-1, child1.resolve_orientation());
  EXPECT_EQ(1, child2.resolve_orientation());
}
}
