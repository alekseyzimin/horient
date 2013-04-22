#include <gtest/gtest.h>
#include <edge_list.hpp>
#include <string> 
#include <fstream>
//#include <sstream>
#include <horient.hpp>
#include <iostream>

namespace {
class merge : public ::testing::Test {
public:
  merge() : master_edge(node_info::sorted_edge_list_type::master_list)
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

  std::list<edge>& master_edge;
  node_map_type    master_node;
};

TEST_F(merge, fe_graph) {
    //This should test merging when edge is first on both nodes.
   const std::string content("0 1 4 0 0 0\n"
			     "1 2 3 0 0 0\n"
			     "0 3 2 3 0 0\n"
			     );
   read_example(content);

    edge_ptr tmp_ptr=master_edge.end();
    --tmp_ptr; //point to edge 0 1 4 0 0 0

    //confirm correct pointing.
    EXPECT_EQ(4, tmp_ptr->good);

    // std::cerr<< "Merging on edge: "<< *tmp_ptr<<"\n";
    // std::cerr<< "Node sent into merge: "<< tmp_ptr->n2<<"\n";
    
    // std::cerr << "Before merge:\n";
    // for(auto it=master_edge.begin();it !=master_edge.end();++it){
    //   std::cerr<< *it <<"\n";
    // }

    // std::cerr << "Local List receiver:\n";
    // for(auto it=tmp_ptr->n1->edges.local_list.begin();
    // 	it !=tmp_ptr->n1->edges.local_list.end();++it){
    //   std::cerr<< **it <<"\n";
    // } 

    // std::cerr << "Local List sender:\n";
    // for(auto it=tmp_ptr->n2->edges.local_list.begin();
    // 	it !=tmp_ptr->n2->edges.local_list.end();++it){
    //   std::cerr<< **it <<"\n";
    // }

    //Merge "1" into "0"
    tmp_ptr->n1->merge(tmp_ptr->n2);

    //Post condition expected:
    // edge: 0->1 gone. --> moved to interior.
    //edges from 1 are in or merged into 0
    //merge loss reset.

    // std::cerr << "After merge:\n";
    // for(auto it=master_edge.begin();it !=master_edge.end();++it){
    //   std::cerr<< *it <<"\n";
    // }
    

    //Test post conditions
    EXPECT_EQ( (size_t)2, master_edge.size());
    EXPECT_EQ( (size_t)2, master_node.find("0")->second->edges.local_list.size());
    EXPECT_EQ( (size_t)0, master_node.find("1")->second->edges.local_list.size());

    EXPECT_EQ( 4, master_node.find("0")->second->int_good);
    EXPECT_EQ( 3, master_edge.begin()->good);
    EXPECT_EQ( 0, master_edge.begin()->bad);
    EXPECT_EQ( 2, (++master_edge.begin())->good);
    EXPECT_EQ( 3, (++master_edge.begin())->bad);
  }


  TEST_F(merge, f2_graph){
    //This should test merging with the merge edge appearing 
    //ONLY on n2_edg_it first... i.e. third loop in node's merge function.

    const std::string content("0 1 3 5 0 0\n"
			      "2 3 4 0 0 0\n"
			      "1 3 2 3 0 0\n"
			     );

    read_example(content);
    edge_ptr tmp_ptr=master_edge.begin();

    //Check pointer to edge i want to merge on:
    EXPECT_EQ(2, tmp_ptr->good);

    // std::cerr<< "Receiver: "<<tmp_ptr->n1<<"\n";
    // std::cerr << "Local List receiver:\n";
    // for(auto it=tmp_ptr->n1->edges.local_list.begin();
    // 	it !=tmp_ptr->n1->edges.local_list.end();++it){
    //   std::cerr<< **it <<"\n";
    // } 

    // std::cerr<<"Sender: "<<tmp_ptr->n2<<"\n";
    // std::cerr << "Local List sender:\n";
    // for(auto it=tmp_ptr->n2->edges.local_list.begin();
    // 	it !=tmp_ptr->n2->edges.local_list.end();++it){
    //   std::cerr<< **it <<"\n";
    // }

    //Merge "3" into "1"
    tmp_ptr->n1->merge(tmp_ptr->n2);

   //Test post conditions
    EXPECT_EQ( (size_t)2, master_edge.size());
    EXPECT_EQ( (size_t)2, master_node.find("1")->second->edges.local_list.size());
    EXPECT_EQ( (size_t)0, master_node.find("3")->second->edges.local_list.size());

    EXPECT_EQ( 2, master_node.find("1")->second->int_good);
    EXPECT_EQ( 4, master_edge.begin()->good);
    EXPECT_EQ( 0, master_edge.begin()->bad);
    EXPECT_EQ( 3, (++master_edge.begin())->good);
    EXPECT_EQ( 5, (++master_edge.begin())->bad);

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
    tmp_ptr = master_edge.begin();

    tmp_ptr->merge_loss=5;

    //    std::cerr << "Before merge:\n";
    for(auto it=master_edge.begin();it !=master_edge.end();++it){
      //      std::cerr<< *it <<"\n";
    }

    //Merge "4" into "2"
    tmp_ptr->n1->merge(tmp_ptr->n2);

    //Post condition expected:
    // edge: 2->4 gone. --> moved to interior.
    //edges from 4 are in or merged into 2
    //merge loss reset.

    //    std::cerr << "After merge:\n";
    for(auto it=master_edge.begin();it !=master_edge.end();++it){
      //      std::cerr<< *it <<"\n";
    }
    
    //Check that all the sizes for lists of edges have changed correctly
    EXPECT_EQ( (size_t)4, master_edge.size());
    EXPECT_EQ( (size_t)2, master_node.find("2")->second->edges.local_list.size());    
    EXPECT_EQ( (size_t)2, master_node.find("1")->second->edges.local_list.size());
    EXPECT_EQ( (size_t)0, master_node.find("4")->second->edges.local_list.size());
 
    //Check that master_edge's edge is changes. && changed all the appropriate values
    tmp_ptr = master_edge.begin();   
    EXPECT_EQ( 5,tmp_ptr->good);
    EXPECT_EQ( 3,tmp_ptr->bad);
    EXPECT_EQ( -1, tmp_ptr->merge_loss);
    
    //Test id's correct
    EXPECT_EQ( 1, tmp_ptr->n1->id);
    EXPECT_EQ( 2, tmp_ptr->n2->id);

    //Test interior updates correctly
    EXPECT_EQ( 5, master_node.find("2")->second->int_good);
    EXPECT_EQ( 0, master_node.find("2")->second->int_bad);

    tmp_ptr= master_edge.end();
    --tmp_ptr; //point to actual last (first) edge.
    --tmp_ptr; //get to edge: 0 2 .....
       
    //    std::cerr << "Before merge:\n";
    for(auto it=master_edge.begin();it !=master_edge.end();++it){
      //      std::cerr<< *it <<"\n";
    }
    
    //Merge "0" into "2"
    //    std::cerr << "Edge merge: " << *tmp_ptr << "\n";
    tmp_ptr->n2->merge(tmp_ptr->n1); //Should merge node "0" into node "2"

    //Post conditions expected:
    //  edge 0->2 gone, interior.
    //  edges on 0 merged or added to 2.

    //    std::cerr << "After merge:\n";
    for(auto it=master_edge.begin();it !=master_edge.end();++it){
      //      std::cerr<< *it <<"\n";
    }

    EXPECT_EQ( (size_t)2, master_edge.size());
    
    auto edg_it= master_edge.begin();
    EXPECT_EQ( 2, edg_it->n1->id);
    EXPECT_EQ( 3, edg_it->n2->id);
    
    
  }

//   TEST(findbest, pf_graph2){
//     //Test on graph used for testing all the pick_flip's.
//     // find best _should_ pick edge: 5 3 0 11 0 0

//     const std::string content("1 2 0 3 0 0\n"
// 			      "4 2 0 6 0 0\n"
// 			      "5 3 0 11 0 0\n"
// 			      "3 6 2 0 0 0\n"
// 			      "3 7 2 0 0 0\n"
// 			      "1 4 3 0 0 0\n"
// 			      "2 5 2 0 0 0\n"
// 			      "2 3 2 0 0 0\n");
    
//     std::istringstream input(content);
//     ASSERT_TRUE(input.good());
//     std::list<edge> master_edge;
//     node_map_type master_node;
//     readdata(master_edge, master_node, true, input);
    
//     edge_ptr best_merge;

//     best_merge= findbestmerge(master_edge);

//     //Iterate to correct edge.

//     // pointer past last in list (top) edge, 1 2 0 3 0 0
//     auto edge_it=master_edge.end(); 

//     --edge_it; //points to edge 1 2 0 3 0 0
//     --edge_it; //points to edge 4 2 0 6 0 0
//     --edge_it; //points to edge 5 3 0 11 0 0

//     //Check that it points to right edge.
//     auto node_it=master_node.find("5");
//     EXPECT_EQ(node_it->second->id, edge_it->n1->id );

//     //Then best_merge should _also_ point to the same edge
//     // if everything is working right.
//     EXPECT_EQ( node_it->second->id, best_merge->n1->id );

//   }

// // TODO: test on more complicated graph.
//   // TEST(findbest, pf_graph3){
//   //   //We should test on some more complicated but knowable graphs. 
//   //   //Adding this blank test so cna re-commit. and so there's
//   //   // at least one error to know not fully tested.

//   //   EXPECT_TRUE(false);
//   // }

//   TEST(Edge, local_removal){
//     //Only tests removal inside a node.

//     //This tests what conditions happen when we delete edges
//     // from various data structures we have. 
//     //?? Do they all point to same edges? Or what's going on??

//     //We want a complex enough data structure to have nodes with
//     //multiple edges, etc....
//     const std::string content("0 1 4 0 0 0\n"
// 			      "0 2 0 2 0 0\n"
// 			      "1 2 6 0 0 0\n"
// 			      "1 3 0 3 0 0\n"
// 			      "2 3 5 0 0 0\n"
// 			      );
    
//     std::istringstream input(content);
//     ASSERT_TRUE(input.good());
//     std::list<edge> master_edge;
//     node_map_type master_node;
//     readdata(master_edge, master_node, true, input);
 
//     auto e_it = master_edge.begin();
//     //auto n_it=master_node.begin();

//     ++e_it; // Should point to 2nd from bottom edge: 1 3 0 3 0 0
//     EXPECT_EQ( 3, e_it->bad); //check pointing to right edge.

//     auto n3_it = master_node.find("3");

//     //Iterator to first pointer to edge for node_3
//     auto node3_first_edge_it = n3_it->second->edges.local_list.begin();

//     //Expect address of the edges pointed to are equal?
//     EXPECT_EQ( &(*e_it), &(*(*node3_first_edge_it)));
//     //Or are the bads equal then?
//     EXPECT_EQ( e_it->bad, (*node3_first_edge_it)->bad);
//     //And can we find it?
//     auto tmp_it= n3_it->second->edges.local_list.begin(); //to get type
//     tmp_it= n3_it->second->edges.local_list.find( e_it );
//     EXPECT_EQ( tmp_it, node3_first_edge_it);

//     //What happens when we erase an edge from a LOCAL_LIST to master?
//     auto n0_it = master_node.find("0");
//     auto n0_edg_it = (*n0_it->second).edges.local_list.begin();

//     //Check this is last edge in master list
//     e_it= master_edge.end();
//     --e_it; //point to last actual edge.

//     //Check we are pointing to same content.
//     EXPECT_EQ(4, e_it->good);
//     EXPECT_EQ(4, (*n0_edg_it)->good);

//     //Now remove it frmo local_list.
//     (*n0_it->second).edges.local_list.erase(n0_edg_it);

//     //Now, reset master iterator, and see what is in same spot.
//     e_it= master_edge.end();
//     --e_it;
    
//     //Actually, this should NOT change the size or elements in master!
//     EXPECT_EQ( (size_t)5 , master_edge.size() );
//     EXPECT_EQ( 4, e_it->good);

//     //Should change size in local list, and beginning.
//     EXPECT_EQ( (size_t)1, (*n0_it->second).edges.local_list.size() );
//     n0_edg_it = (*n0_it->second).edges.local_list.begin();
//     EXPECT_EQ( 2, (*n0_edg_it)->bad);
//   }



//   TEST(Edge, global_removal){
//     //This tests what conditions happen when we delete edges
//     // from various data structures we have. 
//     //?? Do they all point to same edges? Or what's going on??

//     //We want a complex enough data structure to have nodes with
//     //multiple edges, etc....
//     const std::string content("0 1 4 0 0 0\n"
// 			      "0 2 0 2 0 0\n"
// 			      "1 2 6 0 0 0\n"
// 			      "1 3 0 3 0 0\n"
// 			      "2 3 5 0 0 0\n"
// 			      );
    
//     std::istringstream input(content);
//     ASSERT_TRUE(input.good());
//     std::list<edge> master_edge;
//     node_map_type master_node;
//     readdata(master_edge, master_node, true, input);
 
//     auto e_it = master_edge.begin();
//     //auto n_it=master_node.begin();

//     ++e_it; // Should point to 2nd from bottom edge: 1 3 0 3 0 0
//     EXPECT_EQ( 3, e_it->bad); //check pointing to right edge.

//     auto n3_it = master_node.find("3");

//     //Iterator to first pointer to edge for node_3
//     auto node3_first_edge_it = n3_it->second->edges.local_list.begin();

//     //Expect address of the edges pointed to are equal?
//     EXPECT_EQ( &(*e_it), &(*(*node3_first_edge_it)));
//     //Or are the bads equal then?
//     EXPECT_EQ( e_it->bad, (*node3_first_edge_it)->bad);
//     //And can we find it?
//     auto tmp_it= n3_it->second->edges.local_list.begin(); //to get type
//     tmp_it= n3_it->second->edges.local_list.find( e_it );
//     EXPECT_EQ( tmp_it, node3_first_edge_it);


//     //Now, if I erase the edge in the master, what happens?
//     master_edge.erase(e_it);
//     //Master should be smaller now.
//     EXPECT_EQ( (size_t)4 , master_edge.size() );
    
//     //Did the contents get erased? I don't think so....
//     EXPECT_EQ(3, (*node3_first_edge_it)->bad); //Succed if not erased?

//     //The begin iterator should now point to the originally 2nd edge?
//     node3_first_edge_it= n3_it->second->edges.local_list.begin();

//     //This expect FAILS. Meaning the local list begin does not
//     // point to second edge...
//     // EXPECT_EQ(5, (*node3_first_edge_it)->good);

//     //This (failure) proves that local_list thinks it still has 2 elements?
//     // EXPECT_EQ((size_t)1 , n3_it->second->edges.local_list.size());

//     //Is master_edge list smaller? (YES)
//     EXPECT_EQ( (size_t)4 , master_edge.size());

//     e_it = master_edge.begin(); //regenerate beginning
    
//     //Should now point to the edge: 2 3 5 0 0, then 1 2 6 0 0 0
//     EXPECT_EQ( 5, e_it->good);
//     ++e_it; //point to second edge
//     EXPECT_EQ( 6, e_it->good);

    
//     //THE BELOW IS TRYING TO FIGURE OUT WHAT'S GOING ON. 

//     // //So I should be able to add the edge from derefencing local list...
//     // // node3_first_edge_it is a iterator in local_list... so 
//     // // iterator pointing to first 'edge_ptr'
//     // //  so... *node3... is an edge_ptr.
//     // // edge_ptr are iterators INTO lists of edges. SO...
//     // // *(*node3..) should be the location of an edge.
    
//     // //Some cout's to try and figure out what the heck is going on!
//     // //Found a valid, begin iterator...? but gobblygook out..
//     // std::cout<< "good n3 e1: "<<(*(*node3_first_edge_it)).good<<std::endl;
//     // std::cout<< "good n3 e1--b: " <<(*node3_first_edge_it)->good<<std::endl;
//     // std::cout<< "bad n3 e1: "<<(*(*node3_first_edge_it)).bad<<std::endl;
//     // std::cout<< "bad n3 e1--b: " <<(*node3_first_edge_it)->bad<<std::endl;
    
//     // //This turns out to be gobblygook since nothing is in this edge.
//     // master_edge.push_front( *(*node3_first_edge_it) );

//     // //Giving us 5 edges again...
//     // EXPECT_EQ( (size_t)5, master_edge.size() );

//     // //But the first edge should now be: 1 3 0 3 0 0
//     // e_it=master_edge.begin();
//     // //What's the first entry point to??
//     // std::cout<<"bad: "<<e_it->bad<<" good: "<<e_it->good<<std::endl;

//     // std::cout<< "Seg fault when trying to access contents of edge\n";
//     // std::cout<<"n1: "<< (*(*e_it->n1)).id << " n2: ";
//     // std::cout<< (*(*e_it->n2)).id<< std::endl;

//     // EXPECT_EQ( 3, e_it->bad); // Fails?
//     // EXPECT_EQ( 5, e_it->good);
//     // ++e_it; //Now points to 2nd edge...which is??
//     // std::cout<<"bad: "<<e_it->bad<<" good: "<<e_it->good<<std::endl;
//     // std::cout<<"n1: "<< (*(*e_it->n1)).id << " n2: ";
//     // std::cout<< (*(*e_it->n2)).id<< std::endl;
//     // EXPECT_EQ( 3, e_it->bad);

//     // //And if I change the content using the node's pointer..I change global
//     // (*node3_first_edge_it)->bad=9;
//     // EXPECT_EQ( 9, (*node3_first_edge_it)->bad);
//     // EXPECT_EQ( 9, e_it->bad);

    
//   }

}
