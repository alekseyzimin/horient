#ifndef _NODE_H_
#define _NODE_H_

#include <cassert>

#include <utility>   //for swap;
#include <unordered_map>

#include <edge_list.hpp>
#include <iostream>

struct node;
std::ostream& operator<<(std::ostream& os, const node& n);

struct node {
  int id;
  int int_good, int_bad;
  int size;
  // Hold orientation (1 or -1). If parent == 0, it is absolute,
  // otherwise it is relative to parent.
  int orient;
  node* parent;
  typedef edge_base<node> edge_type;
  typedef sorted_edge_list<edge_type>::edge_ptr edge_ptr;
  typedef sorted_edge_list<edge_type> sorted_edge_list_type;
  sorted_edge_list_type edges;

  node(int id_) : 
    id(id_), int_good(0), int_bad(0), size(1), 
    orient(1), parent(0),
    edges(id_)
  { }

  void add_edge(const edge_ptr& e) { edges.add_edge(e); }

  //function to return neighbor on edge
  int far_id(const edge_ptr& e) const {
    return e->n1.id == id ? e->n2.id : e->n1.id;
  }

  //flips this node
  void flip_node(){
    orient *= -1; //Easy part!

    //Now, iterate through all the edges, and swap their good-bad.
    for(auto edge_it=edges.local_list.begin(); edge_it != edges.local_list.end(); edge_it++){
      std::swap( (*edge_it)->good, (*edge_it)->bad );
      std::swap( (*edge_it)->good2, (*edge_it)->bad2);
    }
  }

  // Traverse the tree upward to the root. Compute the absolute
  // orientation of the current node and all the nodes traversed to
  // the root. The nodes are disconnected from their parent.
  //
  // This is recursive but not tail recursive. It could potentially
  // blow up the stack.
  int resolve_orientation() {
    if(!parent) return orient;
    orient *= parent->resolve_orientation();
    parent  = 0;
    return orient;
  }

  template<typename Iterator>
  void merge_n2_less(node& n_old, node& local, Iterator& n2_edg_it) {

    //Not same. N2's edge neighbor is less.
    //We will increment to next edge in N2, since sorted.
    // But that means we also need to move this edge to N1.
    node& far=(*n2_edg_it)->far_node(n_old);

    edge_type e(local, far,
                (*n2_edg_it)->good, (*n2_edg_it)->bad,
                (*n2_edg_it)->good2, (*n2_edg_it)->bad2);
    edges.master_list.push_front(e);
    edge_ptr ne = edges.master_list.begin();
    add_edge(ne);
    far.add_edge(ne);

    far.edges.local_list.erase(*n2_edg_it);
    edges.master_list.erase(*n2_edg_it);
    n2_edg_it = n_old.edges.local_list.erase(n2_edg_it);
  }

  template<typename Iterator>
  void merge_same_neighbor(node& n_old, Iterator& n1_edg_it, Iterator& n2_edg_it) {
    //    node& e1_local_node = (*n1_edg_it)->local_node(this->id);
    node& e2_far_node         = (*n2_edg_it)->far_node(n_old);
    (*n1_edg_it)->good       += (*n2_edg_it)->good;
    (*n1_edg_it)->good2      += (*n2_edg_it)->good2;
    (*n1_edg_it)->bad        += (*n2_edg_it)->bad;
    (*n1_edg_it)->bad2       += (*n2_edg_it)->bad2;
    (*n1_edg_it)->merge_loss  = -1;

    e2_far_node.edges.local_list.erase(*n2_edg_it);
    edges.master_list.erase(*n2_edg_it);
    e2_far_node.edges.local_list.insert(*n1_edg_it);

    n2_edg_it = n_old.edges.local_list.erase(n2_edg_it);
  }

  //Function to merge: Assumption that we pass the CORRECT node which will become child. 
  void merge(node& n_old){
    //Have to move shared edge interior.
    //Have to update size
    //Have to merge int_good, int_bad.
    //Have to merge edges.

    //Update interior int's
    size     += n_old.size; 
    int_good += n_old.int_good; 
    int_bad  += n_old.int_bad;

    //Keep track of relative orientation forest. Orientation in n_old
    //becomes relative to its parent.
    n_old.parent = this;
    n_old.orient = (this->orient == n_old.orient) ? 1 : -1;

    //Cycle through all edges in both nodes, take one of three actions:
    // If edge between them, move interior. -- delete edge
    // If shared neighbor, merge them.
    // If not shared neighbor, 
    //       if self, reset merge_loss
    //       if in n_old, reset merge_loss, relabel near_id, then add to local list.
 

    // These should be iterator which dereference to an edge_ptr.
    //To reduce editing labels below... n1= self, n2= n_old.
    auto n1_edg_it  = edges.local_list.begin();
    auto n2_edg_it  = n_old.edges.local_list.begin();
    auto n1_edg_end = edges.local_list.end();
    auto n2_edg_end = n_old.edges.local_list.end();

    //Local node (self) never changes
    node& e1_local_node = (*n1_edg_it)->local_node(*this);

    //Not for loop because we increment independently.
    // should loop through the two sorted edge lists interior to the two nodes.
    while( n1_edg_it != n1_edg_end && n2_edg_it != n2_edg_end ) {
      //Need these inside the loops, because we change 
      // the FAR node in e2 (*n2_edg_it) each time!
      node& e2_far_node = (*n2_edg_it)->far_node(n_old);

      //If edge is identical, it's the merge edge.
      // Move interior. Zero. Go to next loop

      //Check if one merging edge for both iterators. If so, change both

      if( comp_edge(*n1_edg_it, *n2_edg_it) ){
        int_good+= (*n1_edg_it)->good;
        int_bad+=  (*n1_edg_it)->bad;
        
        edges.master_list.erase(*n1_edg_it);
        n1_edg_it = edges.local_list.erase(n1_edg_it);
        n2_edg_it = n_old.edges.local_list.erase(n2_edg_it);
        continue;
      }

      //Check if on merging edge for iterator n1_edg_it, update it.
      if( (*n1_edg_it)->far_node(e1_local_node).id == n_old.id) {
          int_good+= (*n1_edg_it)->good;
          int_bad+=  (*n1_edg_it)->bad;

          n_old.edges.local_list.erase(*n1_edg_it);
          edges.master_list.erase(*n1_edg_it);
          n1_edg_it = edges.local_list.erase(n1_edg_it);
          continue;
      }
      
      //Check if on merging edge for iterator n2_edg_it, update it.
      if( e2_far_node.id == id) {
	int_good+= (*n2_edg_it)->good;
	int_bad+=  (*n2_edg_it)->bad;
	
	edges.local_list.erase(*n2_edg_it);
	edges.master_list.erase(*n2_edg_it);
	n2_edg_it = n_old.edges.local_list.erase(n2_edg_it);
	continue;

      }

      //Check if nodes point to different edges.

      //Not same. N2's edge neighbor is less. 
      //We will increment to next edge in N2, since sorted.
      // But that means we also need to move this edge to N1.
  
      if( far_id(*n1_edg_it) > e2_far_node.id) {
        merge_n2_less(n_old, e1_local_node, n2_edg_it);
        continue;
      }

      //Not same neighbor, N1's edge neighbor is less. Since sorted,
      // increment N1. Reset merge_loss and go to next edge.
      if( far_id(*n1_edg_it) < n_old.far_id(*n2_edg_it) ) {
        (*n1_edg_it)->merge_loss = -1;
        ++n1_edg_it;
        continue;
      }

      //If we didn't loop yet, we must have Same neighbor. Confirm.
      assert( far_id(*n1_edg_it) == n_old.far_id(*n2_edg_it) );

      merge_same_neighbor(n_old, n1_edg_it, n2_edg_it);

      //Increment one iterator.
      ++n1_edg_it;
    }

    // Drain remaining iterator, either n1_edg_it or n2_edg_it
    while(n1_edg_it != n1_edg_end) {
      (*n1_edg_it)->merge_loss = -1;
      ++n1_edg_it;
    }
    while(n2_edg_it != n2_edg_end) {
      merge_n2_less(n_old, e1_local_node, n2_edg_it);
      //      ++n2_edg_it;
    }
  }
};

inline std::ostream& operator<<(std::ostream& os, const node& n) {
  return os << "<id:" << n.id << " good:" << n.int_good << " bad:" << n.int_bad
            << " size:" << n.size << " edges_size:" << n.edges.local_list.size() << ">";
}

typedef std::unordered_map<std::string, node> node_map_type;

#endif /* _NODE_H_ */
