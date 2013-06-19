#include <string>
#include <fstream>
#include <iostream>

#include <gtest/gtest.h>
#include <horient.hpp>

namespace {

TEST(pickflip, Small) {
  const std::string content("1 2 0 3 0 0\n"
                            "4 2 0 6 0 0\n"
                            "5 3 0 11 0 0\n"
                            "3 6 2 0 0 0\n"
                            "3 7 2 0 0 0\n"
			    "1 4 3 0 0 0\n"
			    "2 5 2 0 0 0\n"
			    "2 3 2 0 0 0\n");

  std::istringstream input(content);
  ASSERT_TRUE(input.good());
  master_list_type master_edge;
  node_map_type master_node;
  readdata(master_edge, master_node, true, input);

  // should return pointer to first node in master list (these are
  // reverse order of above)
  auto it = find_edge(master_edge, "2", "3");

  { // Test 1st edge
    // Picks node to flip on edge  
    node& picked = pick_flip(it);

    // This should pick up condition: n2_bad>n1_bad && n1diff == n2diff
    auto it2 = master_node.find("3");

    // Checks to make sure we found the right node to check out pick
    // logic against
    EXPECT_EQ(it->n2.id, it2->second.id ); 
    EXPECT_EQ( it2->second.id, picked.id);
  }

  { // Test 2nd edge
    it = find_edge(master_edge, "2", "5");
    auto it2= master_node.find("5");

    // Checks to make sure we found the right node to check out pick
    // logic against
    EXPECT_EQ( it->n2.id, it2->second.id );

    // This should pick up the condition: n1diff> n2diff && n1_good !=0
    node& picked=pick_flip(it);
    EXPECT_EQ(it2->second.id, picked.id);
  }

  { // Test 3rd edge
    it = find_edge(master_edge, "1", "4");
    auto it2=master_node.find("4");

    // Checks to make sure we found the right node to check out pick
    // logic against
    EXPECT_EQ(it->n2.id, it2->second.id ); 

    // This should pick up the condition: 
    //     n2_bad>n1_bad && n1_good == n2_good == 0
    node& picked = pick_flip(it);
    EXPECT_EQ( it2->second.id,picked.id);
  }

  { // Test 6th edge
    //This should default to picking the first node in the edge.
    it = find_edge(master_edge, "3", "5");
    auto it2=master_node.find("5" );

    //Checks to make sure we found the right node 
    // to check out pick logic against
    EXPECT_EQ(it->n1.id, it2->second.id); 

    node& picked=pick_flip(it);
    EXPECT_EQ( it2->second.id,picked.id);
  }
}


TEST(pickflip, lock) {
  const std::string content("1 2 0 3 0 0\n"
                            "4 2 0 6 0 0\n"
                            "5 3 0 11 0 0\n"
                            "3 6 2 0 0 0\n"
                            "3 7 2 0 0 0\n"
			    "1 4 3 0 0 0\n"
			    "2 5 2 0 0 0\n"
			    "2 3 2 0 0 0\n");

  std::istringstream input(content);
  ASSERT_TRUE(input.good());
  master_list_type master_edge;
  node_map_type master_node;
  readdata(master_edge, master_node, true, input);

  //Set two nodes to unflippable.
  node_map_type::iterator indx;
  indx=master_node.find("4");
  indx->second.flippable=false;
  indx=master_node.find("3");
  indx->second.flippable=false;

  // should return pointer to first node in master list (these are
  // reverse order of above)
  auto it = find_edge(master_edge, "2", "3");

  { // Test locked 1st node 
    it = find_edge(master_edge, "4", "2");
    auto it2= master_node.find("2");

    // Checks to make sure we found the right node to check out pick
    // logic against
    EXPECT_EQ( it->n2.id, it2->second.id );

    //Node '4' is locked, so should flip node '2'
    node& picked=pick_flip(it);
    EXPECT_EQ(it2->second.id, picked.id);
  }

  { // Test locked 2nd node
    it = find_edge(master_edge, "5", "3");
    auto it2=master_node.find("5");

    // Checks to make sure we found the right node to check out pick
    // logic against
    EXPECT_EQ(it->n1.id, it2->second.id ); 

    //Node '3' is locked, so should flip node '5'
    node& picked = pick_flip(it);
    EXPECT_EQ( it2->second.id,picked.id);
  }

}

}
