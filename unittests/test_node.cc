#include <gtest/gtest.h>
#include <node.hpp>

namespace {
TEST(Node, Simple) {
  node n;

  EXPECT_EQ(0, n->int_good);
  EXPECT_EQ(0, n->int_bad);
  EXPECT_EQ(1, n->size);
}
}
