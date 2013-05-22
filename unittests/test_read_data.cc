#include <string>
#include <fstream>


#include <gtest/gtest.h>
#include <horient.hpp>

namespace {
TEST(ReadData, Small) {
  const std::string content("0 1 7 0 0 0\n" // 7
                            "1 2 4 0 0 0\n" // 4
                            "1 3 0 6 0 0\n" // 6
                            "1 4 1 8 0 0\n" // 8
                            "2 3 1 0 0 0\n"
                            "3 4 5 1 0 0\n" // 5
                            "4 2 0 1 0 0\n"
                            "5 6 2 0 0 0\n" // 1 / 5
                            "6 5 0 3 0 0\n"); // duplicate);

  std::istringstream input(content);
  ASSERT_TRUE(input.good());
  master_list_type master_edge;
  node_map_type master_node;
  readdata(master_edge, master_node, true, input);

  EXPECT_EQ((size_t)7, master_node.size());
  EXPECT_EQ((size_t)6, master_edge.size());
  for(auto it = master_node.cbegin(); it != master_node.cend(); ++it)
    EXPECT_EQ(std::stoi(it->first), it->second.id);

  {
    auto it = master_edge.cbegin();

    EXPECT_NE(it, master_edge.cend());
    EXPECT_EQ(1, it->n1.id);
    EXPECT_EQ(4, it->n2.id);
    EXPECT_EQ(0, it->good);
    EXPECT_EQ(8, it->bad);

    ++it;
    EXPECT_NE(it, master_edge.cend());
    EXPECT_EQ(0, it->n1.id);
    EXPECT_EQ(1, it->n2.id);
    EXPECT_EQ(7, it->good);
    EXPECT_EQ(0, it->bad);

    ++it;
    EXPECT_NE(it, master_edge.cend());
    EXPECT_EQ(1, it->n1.id);
    EXPECT_EQ(3, it->n2.id);
    EXPECT_EQ(0, it->good);
    EXPECT_EQ(6, it->bad);

    ++it;
    EXPECT_NE(it, master_edge.cend());
    EXPECT_EQ(3, it->n1.id);
    EXPECT_EQ(4, it->n2.id);
    EXPECT_EQ(5, it->good);
    EXPECT_EQ(0, it->bad);

    ++it;
    EXPECT_NE(it, master_edge.cend());
    EXPECT_EQ(1, it->n1.id);
    EXPECT_EQ(2, it->n2.id);
    EXPECT_EQ(4, it->good);
    EXPECT_EQ(0, it->bad);

    ++it;
    EXPECT_NE(it, master_edge.cend());
    EXPECT_EQ(5, it->n1.id);
    EXPECT_EQ(6, it->n2.id);
    EXPECT_EQ(2, it->good);
    EXPECT_EQ(3, it->bad);

    ++it;
    EXPECT_EQ(master_edge.cend(), it);
  }

  {
    auto it = master_node.find("0");
    EXPECT_NE(master_node.end(), it);
    auto edges = it->second.edges.local_list;
    EXPECT_EQ((size_t)1, edges.size());
    auto zero_edge = find_edge(master_edge,0,1) ;
    EXPECT_EQ(*edges.begin(), zero_edge);
  }

  {
    auto it = master_node.find("1");
    EXPECT_NE(master_node.end(), it);
    auto edges = it->second.edges.local_list;
    EXPECT_EQ((size_t)4, edges.size());
    auto edge_it = edges.begin();
    ASSERT_NE(edge_it, edges.end());
    auto zero_edge = find_edge(master_edge,0,1) ;
    EXPECT_EQ(*edges.begin(), zero_edge);

    ++edge_it;
    ASSERT_NE(edge_it, edges.end());
    auto second_edge = find_edge(master_edge,1,2);
    EXPECT_EQ(*edge_it, second_edge);

    ++edge_it;
    ASSERT_NE(edge_it, edges.end());
    auto third_edge = find_edge(master_edge,1,3);
    EXPECT_EQ(*edge_it, third_edge);

    ++edge_it;
    ASSERT_NE(edge_it, edges.end());
    auto fourth_edge = find_edge(master_edge,1,4);
    EXPECT_EQ(*edge_it, fourth_edge);

    ++edge_it;
    ASSERT_EQ(edge_it, edges.end());
  }
}
}
