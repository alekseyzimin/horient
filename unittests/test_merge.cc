#include <gtest/gtest.h>
#include <string> 
#include <fstream>
#include <horient.hpp>
#include <iostream>

namespace {
class merge : public ::testing::Test {
public:
  merge() : master_edge(node::sorted_edge_list_type::master_list)
  { }
protected:
  virtual void SetUp() {
    master_edge.clear();
  }
  virtual void TearDown() {
    master_edge.clear();
  }

  void read_example(const std::string content) {
    std::istringstream input(content);
    ASSERT_TRUE(input.good());
    readdata(master_edge, master_node, true, input);
  }

  master_list_type& master_edge;
  node_map_type    master_node;
};

TEST_F(merge, fe_graph) {
    //This should test merging when edge is first on both nodes.
   const std::string content("0 1 4 0 0 0\n"
			     "1 2 3 0 0 0\n"
			     "0 3 2 3 0 0\n"
			     );
   read_example(content);

   edge_ptr tmp_ptr=find_edge(master_edge,"0", "1");
    //confirm correct pointing.
    EXPECT_EQ(4, tmp_ptr->good);

    //Merge "1" into "0"
    tmp_ptr->n1.merge(tmp_ptr->n2);

    //Post condition expected:
    // edge: 0->1 gone. --> moved to interior.
    //edges from 1 are in or merged into 0
    //merge loss reset.

    //Test post conditions
    EXPECT_EQ( (size_t)2, master_edge.size());
    EXPECT_EQ( (size_t)2, master_node.find("0")->second.edges.local_list.size());
    EXPECT_EQ( (size_t)0, master_node.find("1")->second.edges.local_list.size());

    EXPECT_EQ( 4, master_node.find("0")->second.int_good);
    {
      auto it = find_edge(master_edge, "0", "2");
      ASSERT_NE(it, master_edge.end());
      EXPECT_EQ( 3, it->good);
      EXPECT_EQ( 0,it->bad);
    }
    {
      auto it = find_edge(master_edge, "0", "3");
      ASSERT_NE(it, master_edge.end());
      EXPECT_EQ( 2, it->good);
      EXPECT_EQ( 3, it->bad);
    }
  }


  TEST_F(merge, f2_graph){
    //This should test merging with the merge edge appearing 
    //ONLY on n2_edg_it first... i.e. third loop in node's merge function.

    const std::string content("0 1 3 5 0 0\n"
			      "2 3 4 0 0 0\n"
			      "1 3 2 3 0 0\n"
			     );

    read_example(content);
    edge_ptr tmp_ptr=find_edge(master_edge, "1", "3");

    //Check pointer to edge i want to merge on:
    EXPECT_EQ(2, tmp_ptr->good);

    //Merge "3" into "1"
    tmp_ptr->n1.merge(tmp_ptr->n2);

   //Test post conditions
    EXPECT_EQ( (size_t)2, master_edge.size());
    EXPECT_EQ( (size_t)2, master_node.find("1")->second.edges.local_list.size());
    EXPECT_EQ( (size_t)0, master_node.find("3")->second.edges.local_list.size());

    {
      auto it = find_edge(master_edge, "1", "2");
      ASSERT_NE(it, master_edge.end());
      EXPECT_EQ( 2, master_node.find("1")->second.int_good);
      EXPECT_EQ( 4, it->good);
      EXPECT_EQ( 0, it->bad);
    }
    {
      auto it = find_edge(master_edge, "0", "1");
      ASSERT_NE(it, master_edge.end());
      EXPECT_EQ( 3, it->good);
      EXPECT_EQ( 5, it->bad);
    }
  }


  TEST_F(merge, pf_graph1){
    //Tests on 4-node demonstration/example error graph 
    // for choosing least loss.
    
    const std::string content("0 1 4 0 0 0\n"
			      "0 2 0 3 0 0\n"
			      "0 3 0 6 0 0\n"
                              "1 2 5 0 0 0\n"
			      "1 4 0 3 0 0\n"
			      "2 4 5 0 0 0\n"
			      );
    //After first merge have left:  ( 0->1 0->2 0->3 1->2)

    read_example(content);
    edge_ptr tmp_ptr,best_merge;
    
    //At first, just test merging on the last edge.
    tmp_ptr = find_edge(master_edge, "2", "4");

    tmp_ptr->merge_loss=5;

    //Merge "4" into "2"
    tmp_ptr->n1.merge(tmp_ptr->n2);

    //Post condition expected:
    // edge: 2->4 gone. --> moved to interior.
    //edges from 4 are in or merged into 2
    //merge loss reset.

    //Check that all the sizes for lists of edges have changed correctly
    EXPECT_EQ( (size_t)4, master_edge.size());
    EXPECT_EQ( (size_t)2, master_node.find("2")->second.edges.local_list.size());    
    EXPECT_EQ( (size_t)2, master_node.find("1")->second.edges.local_list.size());
    EXPECT_EQ( (size_t)0, master_node.find("4")->second.edges.local_list.size());
 
    //Check that master_edge's edge is changes. && changed all the appropriate values
    tmp_ptr = find_edge(master_edge, "1", "2");
    EXPECT_EQ( 5,tmp_ptr->good);
    EXPECT_EQ( 3,tmp_ptr->bad);
    EXPECT_EQ( -1, tmp_ptr->merge_loss);
    
    //Test id's correct
    EXPECT_EQ( 1, tmp_ptr->n1.id);
    EXPECT_EQ( 2, tmp_ptr->n2.id);

    //Test interior updates correctly
    EXPECT_EQ( 5, master_node.find("2")->second.int_good);
    EXPECT_EQ( 0, master_node.find("2")->second.int_bad);

    tmp_ptr= find_edge(master_edge, "0", "2");

    //Merge "0" into "2"
    tmp_ptr->n2.merge(tmp_ptr->n1); //Should merge node "0" into node "2"

    //Post conditions expected:
    //  edge 0->2 gone, interior.
    //  edges on 0 merged or added to 2.

    EXPECT_EQ( (size_t)2, master_edge.size());
  }


}
