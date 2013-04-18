#include <gtest/gtest.h>
#include <disjoint_set.hpp>

namespace {
typedef disjoint_set<int> node;
TEST(DisjointSet, Balanced) {
  static const int size = 1024;
  node ary[size];

  for(int i = 0; i < size; ++i) {
    EXPECT_EQ(&ary[i], ary[i].find());
    EXPECT_EQ((unsigned int)0, ary[i].rank);
  }

  unsigned int h = 1;
  for(int j = 2; j <= size; j *= 2, ++h) {
    for(int i = 0; i < size; i += j)
      union_set(ary[i], ary[i + j / 2]);
    for(int i = 0; i < size; ++i) {
      EXPECT_EQ(ary[j * (i / j)].find(), ary[i].find());
      EXPECT_EQ(h, ary[i].find()->rank);
    }
  }

  node* root = ary[0].find();
  EXPECT_EQ(h - 1, root->rank);
  for(int i = 0; i < size; ++i)
    EXPECT_EQ(root, ary[i].find());
}
}
