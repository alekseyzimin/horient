#include <string>
// #include <sstream>
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
  std::list<edge> master_edge;
  node_map_type master_node;
  readdata(master_edge, master_node, true, input);

  // should return pointer to first node in master list (these are
  // reverse order of above)
  auto it = master_edge.begin();

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
    ++it;
    auto it2= master_node.find("5");

    // Checks to make sure we found the right node to check out pick
    // logic against
    EXPECT_EQ( it->n2.id, it2->second.id );

    // This should pick up the condition: n1diff> n2diff && n1_good !=0
    node& picked=pick_flip(it);
    EXPECT_EQ(it2->second.id, picked.id);
  }

  { // Test 3rd edge
    ++it; //test the 3rd edge
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
    it++;it++;it++;//6th Edge: 5 3 0 11 0 0
    auto it2=master_node.find("5" );

    //Checks to make sure we found the right node 
    // to check out pick logic against
    EXPECT_EQ(it->n1.id, it2->second.id); 

    node& picked=pick_flip(it);
    EXPECT_EQ( it2->second.id,picked.id);
  }
}
}
