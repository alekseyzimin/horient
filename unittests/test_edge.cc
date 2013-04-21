#include <gtest/gtest.h>
#include <edge_list.hpp>
#include <string> 
#include <fstream>
//#include <sstream>
#include <horient.hpp>

namespace {
  

TEST(Edge, Simple) {
  //  edge::score_function = diff_square; // This is the default

  typedef edge_base<int> edge_i;

  int n0 = 0, n1 = 1;//, n2 = 2, n3 = 3;
  edge_i e0(&n0, &n1, 2, 0);
  edge_i e1(&n1, &n0, 1, 2);

  EXPECT_EQ(2.0, e0.score());
  EXPECT_FLOAT_EQ(1.0 / 3.0, e1.score());
}

  TEST(Find_loss, Simple){

  const std::string content("0 1 4 0 0 0\n"
                            "0 2 0 3 0 0\n"
                            "1 2 5 0 0 0\n"
                            "1 3 0 3 0 0\n"
                            "2 3 5 0 0 0\n"
                            );

  std::istringstream input(content);
  ASSERT_TRUE(input.good());
  std::list<edge> master_edge;
  node_map_type master_node;
  readdata(master_edge, master_node, true, input);
  

  //Test last, 2nd to last, and 3rd to last edge for their expected loss
  EXPECT_EQ(3, find_loss(master_edge.begin()));
  EXPECT_EQ(0, find_loss(++(master_edge.begin()) ) );
  EXPECT_EQ(6, find_loss(++(++(master_edge.begin()) )) );

  }
}
