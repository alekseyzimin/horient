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
    edge_i e0(n0, n1, 2, 0);
    edge_i e1(n1, n0, 1, 2);
    
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

  TEST(far_node, simple){
  
    const std::string content("1 2 0 3 0 0\n");

    std::istringstream input(content);
    ASSERT_TRUE(input.good());

    std::list<edge> master_edge;
    node_map_type master_node;
    readdata(master_edge, master_node, true, input);

    EXPECT_EQ( (size_t)2,master_node.size() );  

    //auto node_it=master_node.begin();
    auto edg_it =master_edge.begin();

    EXPECT_EQ(edg_it->n2.id, edg_it->far_node(edg_it->n1).id);
    
  }


  TEST(Find_loss, Simple){
    //In find_loss the 3 edges tested should test the conditions in
    // line:
    // (diff_n1_edg < 0 || diff_n2_edg < 0 ) 
    //         && (diff_n1_edg > 0 || diff_n2_edg > 0 )

    // Edge 1 (last) should generate diff_n1_edg>0 && diff_n2_edg<0
    // Edge 2 (2nd to last) tests finding a loss when a flip occurs.
    //   generates: diff_n1_edg>0 && diff_n2_edg<0
    // Edge 3 (middle) should generate two sets of :
    //      the diff edges. 
    // Edge 4 should flip node 0, and then generate:
    //  diff_n1_edg<0 && diff_n2_edg>0

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
  //Edge: 2 3 5 0 0 0
  EXPECT_EQ(-1, it->merge_loss);
  EXPECT_EQ(3, find_loss(it));
  EXPECT_EQ(3, it->merge_loss);

  ++it; //Edge 1 3 0 3 0 0
  EXPECT_EQ(-1, it->merge_loss);
  EXPECT_EQ(5, find_loss(it) );
  EXPECT_EQ(5, it->merge_loss);

  ++it; //Edge 1 2 5 0 0 0
  EXPECT_EQ(-1, it->merge_loss);
  EXPECT_EQ(6, find_loss(it) );
  EXPECT_EQ(6, it->merge_loss);

  ++it; //Edge 0 2 0 3 0 0
  EXPECT_EQ(-1, it->merge_loss);

  //Also check that node orient is restore to original!
  EXPECT_EQ( 1, it->n1.orient);

  //Find loss, which should include flip.
  EXPECT_EQ(4, find_loss(it) );
  EXPECT_EQ(4, it->merge_loss);

  //Check that orient doesn't leave changed.
  EXPECT_EQ(1, it->n1.orient);

  ++it; //EDGE 0 1 4 0 0 0
  EXPECT_EQ(-1, it->merge_loss);

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

  TEST(findbest, pf_graph1){
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

  TEST(findbest, pf_graph2){
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
    EXPECT_EQ(node_it->second.id, edge_it->n1.id );

    //Then best_merge should _also_ point to the same edge
    // if everything is working right.
    EXPECT_EQ( node_it->second.id, best_merge->n1.id );

  }

// TODO: test on more complicated graph.
  // TEST(findbest, pf_graph3){
  //   //We should test on some more complicated but knowable graphs. 
  //   //Adding this blank test so cna re-commit. and so there's
  //   // at least one error to know not fully tested.

  //   EXPECT_TRUE(false);
  // }

  TEST(Edge, local_removal){
    //Only tests removal inside a node.

    //This tests what conditions happen when we delete edges
    // from various data structures we have. 
    //?? Do they all point to same edges? Or what's going on??

    //We want a complex enough data structure to have nodes with
    //multiple edges, etc....
    const std::string content("0 1 4 0 0 0\n"
			      "0 2 0 2 0 0\n"
			      "1 2 6 0 0 0\n"
			      "1 3 0 3 0 0\n"
			      "2 3 5 0 0 0\n"
			      );
    
    std::istringstream input(content);
    ASSERT_TRUE(input.good());
    std::list<edge> master_edge;
    node_map_type master_node;
    readdata(master_edge, master_node, true, input);
 
    auto e_it = master_edge.begin();
    //auto n_it=master_node.begin();

    ++e_it; // Should point to 2nd from bottom edge: 1 3 0 3 0 0
    EXPECT_EQ( 3, e_it->bad); //check pointing to right edge.

    auto n3_it = master_node.find("3");

    //Iterator to first pointer to edge for node_3
    auto node3_first_edge_it = n3_it->second.edges.local_list.begin();

    //Expect address of the edges pointed to are equal?
    EXPECT_EQ( &(*e_it), &(*(*node3_first_edge_it)));
    //Or are the bads equal then?
    EXPECT_EQ( e_it->bad, (*node3_first_edge_it)->bad);
    //And can we find it?
    auto tmp_it= n3_it->second.edges.local_list.begin(); //to get type
    tmp_it= n3_it->second.edges.local_list.find( e_it );
    EXPECT_EQ( tmp_it, node3_first_edge_it);

    //What happens when we erase an edge from a LOCAL_LIST to master?
    auto n0_it = master_node.find("0");
    auto n0_edg_it = n0_it->second.edges.local_list.begin();

    //Check this is last edge in master list
    e_it= master_edge.end();
    --e_it; //point to last actual edge.

    //Check we are pointing to same content.
    EXPECT_EQ(4, e_it->good);
    EXPECT_EQ(4, (*n0_edg_it)->good);

    //Now remove it frmo local_list.
    n0_it->second.edges.local_list.erase(n0_edg_it);

    //Now, reset master iterator, and see what is in same spot.
    e_it= master_edge.end();
    --e_it;
    
    //Actually, this should NOT change the size or elements in master!
    EXPECT_EQ( (size_t)5 , master_edge.size() );
    EXPECT_EQ( 4, e_it->good);

    //Should change size in local list, and beginning.
    EXPECT_EQ( (size_t)1, n0_it->second.edges.local_list.size() );
    n0_edg_it = n0_it->second.edges.local_list.begin();
    EXPECT_EQ( 2, (*n0_edg_it)->bad);
  }



  TEST(Edge, global_removal){
    //This tests what conditions happen when we delete edges
    // from various data structures we have. 
    //?? Do they all point to same edges? Or what's going on??

    //We want a complex enough data structure to have nodes with
    //multiple edges, etc....
    const std::string content("0 1 4 0 0 0\n"
			      "0 2 0 2 0 0\n"
			      "1 2 6 0 0 0\n"
			      "1 3 0 3 0 0\n"
			      "2 3 5 0 0 0\n"
			      );
    
    std::istringstream input(content);
    ASSERT_TRUE(input.good());
    std::list<edge> master_edge;
    node_map_type master_node;
    readdata(master_edge, master_node, true, input);
 
    auto e_it = master_edge.begin();
    //auto n_it=master_node.begin();

    ++e_it; // Should point to 2nd from bottom edge: 1 3 0 3 0 0
    EXPECT_EQ( 3, e_it->bad); //check pointing to right edge.

    auto n3_it = master_node.find("3");

    //Iterator to first pointer to edge for node_3
    auto node3_first_edge_it = n3_it->second.edges.local_list.begin();

    //Expect address of the edges pointed to are equal?
    EXPECT_EQ( &(*e_it), &(*(*node3_first_edge_it)));
    //Or are the bads equal then?
    EXPECT_EQ( e_it->bad, (*node3_first_edge_it)->bad);
    //And can we find it?
    auto tmp_it= n3_it->second.edges.local_list.begin(); //to get type
    tmp_it= n3_it->second.edges.local_list.find( e_it );
    EXPECT_EQ( tmp_it, node3_first_edge_it);


    //Now, if I erase the edge in the master, what happens?
    master_edge.erase(e_it);
    //Master should be smaller now.
    EXPECT_EQ( (size_t)4 , master_edge.size() );
    
    //Did the contents get erased? I don't think so....
    EXPECT_EQ(3, (*node3_first_edge_it)->bad); //Succed if not erased?

    //The begin iterator should now point to the originally 2nd edge?
    node3_first_edge_it= n3_it->second.edges.local_list.begin();

    //This expect FAILS. Meaning the local list begin does not
    // point to second edge...
    // EXPECT_EQ(5, (*node3_first_edge_it)->good);

    //This (failure) proves that local_list thinks it still has 2 elements?
    // EXPECT_EQ((size_t)1 , n3_it->second.edges.local_list.size());

    //Is master_edge list smaller? (YES)
    EXPECT_EQ( (size_t)4 , master_edge.size());

    e_it = master_edge.begin(); //regenerate beginning
    
    //Should now point to the edge: 2 3 5 0 0, then 1 2 6 0 0 0
    EXPECT_EQ( 5, e_it->good);
    ++e_it; //point to second edge
    EXPECT_EQ( 6, e_it->good);

    
    //THE BELOW IS TRYING TO FIGURE OUT WHAT'S GOING ON. 

    // //So I should be able to add the edge from derefencing local list...
    // // node3_first_edge_it is a iterator in local_list... so 
    // // iterator pointing to first 'edge_ptr'
    // //  so... *node3... is an edge_ptr.
    // // edge_ptr are iterators INTO lists of edges. SO...
    // // *(*node3..) should be the location of an edge.
    
    // //Some cout's to try and figure out what the heck is going on!
    // //Found a valid, begin iterator...? but gobblygook out..
    // std::cout<< "good n3 e1: "<<(*(*node3_first_edge_it)).good<<std::endl;
    // std::cout<< "good n3 e1--b: " <<(*node3_first_edge_it)->good<<std::endl;
    // std::cout<< "bad n3 e1: "<<(*(*node3_first_edge_it)).bad<<std::endl;
    // std::cout<< "bad n3 e1--b: " <<(*node3_first_edge_it)->bad<<std::endl;
    
    // //This turns out to be gobblygook since nothing is in this edge.
    // master_edge.push_front( *(*node3_first_edge_it) );

    // //Giving us 5 edges again...
    // EXPECT_EQ( (size_t)5, master_edge.size() );

    // //But the first edge should now be: 1 3 0 3 0 0
    // e_it=master_edge.begin();
    // //What's the first entry point to??
    // std::cout<<"bad: "<<e_it->bad<<" good: "<<e_it->good<<std::endl;

    // std::cout<< "Seg fault when trying to access contents of edge\n";
    // std::cout<<"n1: "<< (*(*e_it->n1)).id << " n2: ";
    // std::cout<< (*(*e_it->n2)).id<< std::endl;

    // EXPECT_EQ( 3, e_it->bad); // Fails?
    // EXPECT_EQ( 5, e_it->good);
    // ++e_it; //Now points to 2nd edge...which is??
    // std::cout<<"bad: "<<e_it->bad<<" good: "<<e_it->good<<std::endl;
    // std::cout<<"n1: "<< (*(*e_it->n1)).id << " n2: ";
    // std::cout<< (*(*e_it->n2)).id<< std::endl;
    // EXPECT_EQ( 3, e_it->bad);

    // //And if I change the content using the node's pointer..I change global
    // (*node3_first_edge_it)->bad=9;
    // EXPECT_EQ( 9, (*node3_first_edge_it)->bad);
    // EXPECT_EQ( 9, e_it->bad);

    
  }

}
