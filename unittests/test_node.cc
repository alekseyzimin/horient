#include <gtest/gtest.h>
#include <node.hpp>
#include <edge_list.hpp>

namespace {
  
  TEST(Node, Simple) {
    node n(0);
    
    EXPECT_EQ(0, n->id);
    EXPECT_EQ(0, n->int_good);
    EXPECT_EQ(0, n->int_bad);
    EXPECT_EQ(1, n->size);
    EXPECT_EQ(1, n->orient);
  }

 
  typedef edge_base<node> edge;
  std::list<edge> master_edge;
  std::list<edge>::iterator edge_ptr;

  TEST(Node, created_2){
 
    //Make 2 nodes and an edge. So we can put together
    node n1(0),n2(1);
    master_edge.push_front(edge(&n1,&n2,2,1));
   
    EXPECT_EQ(0, n1->id);
    EXPECT_EQ(1, n2->id);
  }  

  TEST(Node, add_edge) {
    //Make 2 nodes and an edge. So we can put together
    node n1(0),n2(1);
    master_edge.push_front(edge(&n1,&n2,2,1));
        
    edge_ptr=master_edge.begin();
    n1->add_edge(edge_ptr);   
    EXPECT_EQ((size_t)1, n1->edges.local_list.size() );
  }
    
  TEST(Node, far_id) {
    //Make 2 nodes and an edge. So we can put together
    node n1(0),n2(1);
    master_edge.push_front(edge(&n1,&n2,2,1));
   
    //Primary test of far id. 
    edge_ptr=master_edge.begin();
    n1->add_edge(edge_ptr);   
    EXPECT_EQ((size_t)1, n1->edges.local_list.size() );
    EXPECT_EQ(1, n1->far_id(edge_ptr));

    //Test that we are actually getting the right ID out.
    n2->add_edge(edge_ptr);
    EXPECT_EQ((size_t)1,n2->edges.local_list.size() );
    EXPECT_EQ(0, n2->far_id(edge_ptr));
  }
    

  TEST(Node, flip_node) {
    //This tests the flip_node function of node_info
    node n1(0),n2(1);
    master_edge.push_front(edge(&n1,&n2,2,1,5,3));
    
    //Primary test of far id. 
    edge_ptr=master_edge.begin();
    n1->add_edge(edge_ptr);
    
    //Check IDs
    EXPECT_EQ( 0, (*edge_ptr->n1)->id);
    EXPECT_EQ( 1, (*edge_ptr->n2)->id);
    
    //Check Orients
    EXPECT_EQ( 1, (*edge_ptr->n1)->orient);
    EXPECT_EQ( 1, (*edge_ptr->n2)->orient);
    
    //Check good/bad
    EXPECT_EQ( 2, edge_ptr->good );
    EXPECT_EQ( 1, edge_ptr->bad  );
    EXPECT_EQ( 5, edge_ptr->good2);
    EXPECT_EQ( 3, edge_ptr->bad2 );

    //Try to flip node 1
    n1->flip_node();

    //Orient should be different, and good/bad on edge swapped.
    EXPECT_EQ( -1, (*edge_ptr->n1)->orient);
    EXPECT_EQ( 1, edge_ptr->good );
    EXPECT_EQ( 2, edge_ptr->bad  );
    EXPECT_EQ( 3, edge_ptr->good2);
    EXPECT_EQ( 5, edge_ptr->bad2 );

    //Now flip node 2. 
    n2->flip_node();

    //Note these are all NOT equal. Because we 
    // have not added the edge TO node 2... so it doesn't flip it.

    //Could change orient on node 2, 
    // and return good/bad on edge to original values.
    EXPECT_EQ( -1, (*edge_ptr->n2)->orient);
    EXPECT_NE( 2, edge_ptr->good );
    EXPECT_NE( 1, edge_ptr->bad  );
    EXPECT_NE( 5, edge_ptr->good2);
    EXPECT_NE( 3, edge_ptr->bad2 );

    //If we add the edge to the node we can flip it!
    n2->add_edge(edge_ptr);
    n2->flip_node();
    
    //Should change orient on node 2, (always, this time to original)
    // and return good/bad on edge to original values. (because it's added)
    EXPECT_EQ( 1, (*edge_ptr->n2)->orient);
    EXPECT_EQ( 2, edge_ptr->good );
    EXPECT_EQ( 1, edge_ptr->bad  );
    EXPECT_EQ( 5, edge_ptr->good2);
    EXPECT_EQ( 3, edge_ptr->bad2 );

 }
  
    
}
