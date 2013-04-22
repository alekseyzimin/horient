#ifndef _NODE_H_
#define _NODE_H_

#include <cassert>

#include <utility>   //for swap;
#include <unordered_map>

#include <disjoint_set.hpp>
#include <edge_list.hpp>
#include <iostream>

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
  typedef edge_base<node> edge_type;
  typedef sorted_edge_list<edge_type>::edge_ptr edge_ptr;
  typedef sorted_edge_list<edge_type> sorted_edge_list_type;
  sorted_edge_list_type edges;

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

  //Function to merge: Assumption that we pass the CORRECT node which will become child. 
  void merge(node& n_old){
    //Have to move shared edge interior.
    //Have to update size
    //Have to merge int_good, int_bad.
    //Have to merge edges.

    //Update interior int's
    size     += n_old->size; 
    int_good += n_old->int_good; 
    int_bad  += n_old->int_bad;

    //Cycle through all edges in both nodes, take one of three actions:
    // If edge between them, move interior. -- delete edge
    // If shared neighbor, merge them.
    // If not shared neighbor, 
    //       if self, reset merge_loss
    //       if in n_old, reset merge_loss, relabel near_id, then add to local list.
 

    // These should be iterator which dereference to an edge_ptr.
    //To reduce editing labels below... n1= self, n2= n_old.
    auto n1_edg_it  = edges.local_list.begin();
    auto n2_edg_it  = n_old->edges.local_list.begin();
    auto n1_edg_end = edges.local_list.end();
    auto n2_edg_end = n_old->edges.local_list.end();

    //Not for loop because we increment independently.
    // should loop through the two sorted edge lists interior to the two nodes.
    while(n1_edg_it != n1_edg_end && n2_edg_it != n2_edg_end) {

      std::cout<<"looping in while\n";
      //If edge is identical, it's the merge edge.
      // Move interior. Zero. Go to next loop
      if( comp_edge(*n1_edg_it, *n2_edg_it) ){
          int_good+= (*n1_edg_it)->good;
          int_bad+=  (*n1_edg_it)->bad;

          edges.master_list.erase(*n1_edg_it);
          n1_edg_it = edges.local_list.erase(n1_edg_it);
          n2_edg_it = n_old->edges.local_list.erase(n2_edg_it);
          continue;
        }

      //Check if nodes point to different edges.

      // Not same. N1's edge neighbor is less. Since in sorted,
      // increment to next edge in N1
      //Since we are getting rid of N2/old... we just go to next edge in N1
      if( far_id(*n1_edg_it) < n_old->far_id(*n2_edg_it) ) {++n1_edg_it;continue;}

      std::cout<<"early loop\n";

      node& e1_local_node = (*n1_edg_it)->local_node(this->id);
      //Not same. N2's edge neighbor is less. We will increment to next edge in N2, since sorted.
      // But that means we also need to move this edge to N1.
      node& e2_far_node = (*n2_edg_it)->far_node(n_old);


      if( far_id(*n1_edg_it) > e2_far_node->id) {
        edge_type e(e1_local_node, e2_far_node,
                    (*n2_edg_it)->good, (*n2_edg_it)->bad,
                    (*n2_edg_it)->good2, (*n2_edg_it)->bad2);
        edges.master_list.push_front(e);
        edge_ptr ne = edges.master_list.begin();
        add_edge(ne);
        e2_far_node->add_edge(ne);

        edges.master_list.erase(*n2_edg_it);
        n2_edg_it = n_old->edges.local_list.erase(n2_edg_it);
        e2_far_node->edges.local_list.erase(*n2_edg_it);
        continue;
      }

      std::cout<<"mid loop\n";

      //If we didn't loop yet, we must have Same neighbor. Confirm.
      assert( far_id(*n1_edg_it) == n_old->far_id(*n2_edg_it) );

      //Call merge edge on MY edge.
      //      (*n1_edg_it)->merge(*n2_edg_it);
      (*n1_edg_it)->good       += (*n2_edg_it)->good;
      (*n1_edg_it)->good2      += (*n2_edg_it)->good2;
      (*n1_edg_it)->bad        += (*n2_edg_it)->bad;
      (*n1_edg_it)->bad2       += (*n2_edg_it)->bad2;
      (*n1_edg_it)->merge_loss  = -1;

      std::cout<<"late loop \n";

      edges.master_list.erase(*n2_edg_it);
      e2_far_node->edges.local_list.erase(n2_edg_it);
      e2_far_node->edges.local_list.insert(*n1_edg_it);

      n2_edg_it = n_old->edges.local_list.erase(n2_edg_it);

      //Increment one iterator.
      ++n1_edg_it;

    }

  } 
};

typedef std::unordered_map<std::string, node> node_map_type;

#endif /* _NODE_H_ */
