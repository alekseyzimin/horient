#include <gtest/gtest.h>
#include <node.hpp>

namespace {
TEST(Node, Simple) {
  node n(0);

  EXPECT_EQ(0, n->id);
  EXPECT_EQ(0, n->int_good);
  EXPECT_EQ(0, n->int_bad);
  EXPECT_EQ(1, n->size);
}
}
