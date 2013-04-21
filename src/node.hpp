#ifndef _NODE_H_
#define _NODE_H_

#include <utility>   //for swap;
#include <unordered_map>

#include <disjoint_set.hpp>
#include <edge_list.hpp>

struct node_info;
typedef disjoint_set<node_info> node;

//Function to actually merge two nodes. Does copying and calling of union.
// void node_merge(node* n1, node* n2){

//   (*n1)->merge_ni(n2);

//   union_set(n1,n2);

// }

struct node_info {
  int id;
  int int_good, int_bad;
  int size;
  int orient; //Holds orientation relative to 'parent' .. parent is defined as the FIRST node which becomes the cannonical representative of this. 
  typedef sorted_edge_list<edge_base<node> >::edge_ptr edge_ptr;
  sorted_edge_list<edge_base<node> > edges;

  node_info(int id_) : id(id_), int_good(0), int_bad(0), size(1), orient(1), edges(id_) { }
  void add_edge(const edge_ptr& e) { edges.add_edge(e); }

  //function to return neighbor on edge
  int far_id(const edge_ptr& e) const {
    return e->n1->id == id ? e->n2->id : e->n1->id;
  }

  //flips this node
  void flip_node(){
    orient*=-1; //Easy part!
    
    //Now, iterate through all the edges, and swap their good-bad.
    for(auto edge_it=edges.local_list.begin(); edge_it != edges.local_list.end(); edge_it++){
      std::swap( (*edge_it)->good, (*edge_it)->bad );
      std::swap( (*edge_it)->good2, (*edge_it)->bad2);
    }

  }

  // //Function to merge: Assumption that we pass the CORRECT node which will become child. 
  // void merge(const node& n_old){
   
  //   //Have to move shared edge interior.
  //   //Have to update size
  //   //Have to merge int_good, int_bad.
  //   //Have to merge edges.

  //   //Update interior int's
  //   size += (*n_old).size; 
  //   int_good += (*n_old).int_good; 
  //   int_bad  += (*n_old).int_bad;

  //   //Cycle through all edges in both nodes, take one of three actions:
  //   // If edge between them, move interior. -- ZERO OUT good/bad.
  //   // If shared neighbor, merge them.
  //   // If not shared neighbor, 
  //   //       if self, reset merge_loss
  //   //       if in n_old, reset merge_loss, relabel near_id, then add to local list.
 

  //   // These should be iterator which dereference to an edge_ptr.
  //   //To reduce editing labels below... n1= self, n2= n_old.
  //   auto n1_edg_it = edges.local_list.begin();
  //   auto n2_edg_it = (*n_old).edges.local_list.begin();
  //   auto n1_edg_end = edges.local_list.end();
  //   auto n2_edg_end = (*n_old).edges.local_list.end();

  //   //Get a node pointer which points to ourself for modifying edges.
  //   node& self =
  //     (*n1_edg_it)->n1->id == id ? (*n1_edg_it)->n1 : (*n1_edg_it)->n2;
    
  //   //Not for loop because we increment independently.
  //   // should loop through the two sorted edge lists interior to the two nodes.
  //   while(n1_edg_it != n1_edg_end && n2_edg_it != n2_edg_end) {
  //     //If edge is identical, it's the merge edge.
  //     // Move interior. Zero. Go to next loop
  //     if( comp_edge(*n1_edg_it, *n2_edg_it) ){
  //         int_good+= (*n1_edg_it)->good;
  //         int_bad+=  (*n1_edg_it)->bad;

  //         //These help fix that master node-list still has the edge.
  //         (*n1_edg_it)->good = 0;
  //         (*n1_edg_it)->bad  = 0;
  //         (*n1_edg_it)->good2= 0;
  //         (*n1_edg_it)->bad2 = 0;
  //         (*n1_edg_it)->merge_loss=-1; //Just in case?

  //         //Incrememnt both edge iterators, continue.
  //         ++n1_edg_it; ++n2_edg_it; continue;
  //       }

  //     //Check if nodes point to different edges.
      
  //     //Not same. N1's edge neighbor is less. Since in sorted, increment to next edge in N1
  //     // Since we are getting rid of N2/old... we just go to next edge in N1
  //     if( far_id(*n1_edg_it) < n_old->far_id(*n2_edg_it) ) {++n1_edg_it;continue;}
      
  //     //Not same. N2's edge neighbor is less. We will increment to next edge in N2, since sorted.
  //     // But that means we also need to move this edge to N1.
  //     if( far_id(*n1_edg_it) > n_old->far_id(*n2_edg_it) ) {
  //       //Change values, then just call add_edge function on it.
  //       (*n2_edg_it)->merge_loss=-1;

  //       //This needs to get set to a pointer to the disjoint set this is an element of. 
  //       if( (*n2_edg_it)->n1->id == n_old.id ) { (*n2_edg_it)->n1 = self;}
  //       else{ (*n2_edg_it)->n1 = self;}

  //       add_edge(*n2_edg_it);

  //       ++n2_edg_it;continue;
  //     }

  //     //If we didn't loop yet, we must have Same neighbor. Confirm.
  //     assert( far_id(*n1_edg_it) == n_old->far_id(*n2_edg_it) );

  //     //Call merge edge on MY edge.
  //     (*n1_edg_it)->merge(*n2_edg_it);

  //     //Increment one iterator.
  //     ++n1_edge_it;

  //   }

  // } 
    
  
    
};

typedef std::unordered_map<std::string, node> node_map_type;

#endif /* _NODE_H_ */
