#include <string>
// #include <sstream>
#include <fstream>


#include <gtest/gtest.h>
#include <horient.hpp>

namespace {
TEST(ReadData, Small) {
  const std::string content("0 1 7 0 0 0\n"
                            "1 2 4 0 0 0\n"
                            "1 3 0 6 0 0\n"
                            "1 4 1 8 0 0\n"
                            "2 3 1 0 0 0\n"
                            "3 4 5 1 0 0\n"
                            "4 2 0 1 0 0\n");

  std::istringstream input(content);
  ASSERT_TRUE(input.good());
  std::list<edge> master_edge;
  node_map_type master_node;
  readdata(master_edge, master_node, true, input);

  EXPECT_EQ((size_t)5, master_node.size());
  for(auto it = master_node.cbegin(); it != master_node.cend(); ++it) {
    EXPECT_EQ(std::stoi(it->first), it->second->id);
  }

  auto it = master_edge.cbegin();
  EXPECT_NE(it, master_edge.cend());
  EXPECT_EQ(3, (*it->n1)->id);
  EXPECT_EQ(4, (*it->n2)->id);
  EXPECT_EQ(5, it->good);
  EXPECT_EQ(0, it->bad);

  ++it;
  EXPECT_NE(it, master_edge.cend());
  EXPECT_EQ(1, (*it->n1)->id);
  EXPECT_EQ(4, (*it->n2)->id);
  EXPECT_EQ(0, it->good);
  EXPECT_EQ(8, it->bad);

  ++it;
  EXPECT_NE(it, master_edge.cend());
  EXPECT_EQ(1, (*it->n1)->id);
  EXPECT_EQ(3, (*it->n2)->id);
  EXPECT_EQ(0, it->good);
  EXPECT_EQ(6, it->bad);

  ++it;
  EXPECT_NE(it, master_edge.cend());
  EXPECT_EQ(1, (*it->n1)->id);
  EXPECT_EQ(2, (*it->n2)->id);
  EXPECT_EQ(4, it->good);
  EXPECT_EQ(0, it->bad);

  ++it;
  EXPECT_NE(it, master_edge.cend());
  EXPECT_EQ(0, (*it->n1)->id);
  EXPECT_EQ(1, (*it->n2)->id);
  EXPECT_EQ(7, it->good);
  EXPECT_EQ(0, it->bad);

  ++it;
  EXPECT_EQ(master_edge.cend(), it);
}
}
