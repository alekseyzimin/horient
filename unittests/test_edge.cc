#include <gtest/gtest.h>
#include <edge_list.hpp>

namespace {
typedef edge_base<int> edge;

TEST(Edge, Simple) {
  //  edge::score_function = diff_square; // This is the default

  int n0 = 0, n1 = 1;//, n2 = 2, n3 = 3;
  edge e0(&n0, &n1, 2, 0);
  edge e1(&n1, &n0, 1, 2);

  EXPECT_TRUE(!(e0 < e1) && !(e1 < e0));
  EXPECT_EQ(e0.n1, e1.n1);
  EXPECT_EQ(e0.n2, e1.n2);
  EXPECT_EQ(2.0, e0.score());
  EXPECT_FLOAT_EQ(1.0 / 3.0, e1.score());
}
}
