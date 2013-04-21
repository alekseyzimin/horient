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
  TEST(compedge, simple){
  
    const std::string content("1 2 0 3 0 0\n");

  std::istringstream input(content);
  ASSERT_TRUE(input.good());
  std::list<edge> master_edge;
  node_map_type master_node;
  readdata(master_edge, master_node, true, input);
  EXPECT_EQ( (size_t)2,master_node.size() );  

  EXPECT_TRUE(comp_edge(master_edge.begin(),master_edge.begin()) );

  }

  TEST(Find_loss, Simple){
    //In find_loss the 3 edges tested should test the conditions in
    // line:
    // (diff_n1_edg < 0 || diff_n2_edg < 0 ) 
    //         && (diff_n1_edg > 0 || diff_n2_edg > 0 )

    // Edge 1 (last) should generate diff_n1_edg>0 && diff_n2_edg<0
    // Edge 2 (2nd to last) should generate diff_n1_edg>0 && diff_n2_edg>0
    // Edge 3 (middle) should generate two sets of :
    //      the diff edges. 

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
  

  auto it=master_edge.begin();

  //Test last, 2nd to last, and 3rd to last edge for their expected loss
  EXPECT_EQ(-1, it->merge_loss);
  EXPECT_EQ(3, find_loss(it));
  EXPECT_EQ(3, it->merge_loss);

  ++it;
  EXPECT_EQ(-1, it->merge_loss);
  EXPECT_EQ(5, find_loss(it) );
  EXPECT_EQ(5, it->merge_loss);

  ++it;
  EXPECT_EQ(-1, it->merge_loss);
  EXPECT_EQ(6, find_loss(it) );
  EXPECT_EQ(6, it->merge_loss);

  //Should have far more complex Find Loss. Above just tests
  // the actual comparison that adds. and once.

  // Second thought, it _might_ be ok... since the 3rd edge to 
  // test the find loss on DOES make the joined nodes have 
  // TWO neighbors. And therefore add things.

  //Does not test having two neighbors and NOT adding one of them.
  //Does not test mixed edges combinations. 
  //Not 100% sure we've fully tested the iterating while loop,
  //  which runs through the 2 local_list's interior to edge.

  }

  TEST(findbest, graph1){
    //Tests on 4-node demonstration/example error graph 
    // for choosing least loss.

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
    
    edge_ptr best_merge;
    
    best_merge= findbestmerge(master_edge);
    
    EXPECT_TRUE(comp_edge(best_merge, master_edge.begin()) );
  }

  TEST(findbest, graph2){
    //Test on graph used for testing all the pick_flip's.
    // find best _should_ pick edge: 5 3 0 11 0 0

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
    
    edge_ptr best_merge;

    best_merge= findbestmerge(master_edge);

    //Iterate to correct edge.

    // pointer past last in list (top) edge, 1 2 0 3 0 0
    auto edge_it=master_edge.end(); 

    --edge_it; //points to edge 1 2 0 3 0 0
    --edge_it; //points to edge 4 2 0 6 0 0
    --edge_it; //points to edge 5 3 0 11 0 0

    //Check that it points to right edge.
    auto node_it=master_node.find("5");
    EXPECT_EQ( (*node_it->second).id, (*edge_it->n1)->id );

    //Then best_merge should _also_ point to the same edge
    // if everything is working right.
    EXPECT_EQ( (*node_it->second).id, (*best_merge->n1)->id );

  }

  TEST(findbest, graph3){
    //We should test on some more complicated but knowable graphs. 
    //Adding this blank test so cna re-commit. and so there's
    // at least one error to know not fully tested.

    EXPECT_TRUE(false);
  }

}
