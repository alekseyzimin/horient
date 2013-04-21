#ifndef _NODE_H_
#define _NODE_H_

#include <unordered_map>
#include <disjoint_set.hpp>
#include <edge_list.hpp>
#include <utility>   //for swap;

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
  int far_id(const edge_ptr& e) {return (*e->n1)->id == id ? (*e->n2)->id : (*e->n1)->id;}

  //flips this node
  void flip_node(){
    orient*=-1; //Easy part!
    
    //Now, iterate through all the edges, and swap their good-bad.
    for(auto edge_it=edges.local_list.begin(); edge_it != edges.local_list.end(); edge_it++){
      std::swap( (*edge_it)->good, (*edge_it)->bad );
      std::swap( (*edge_it)->good2, (*edge_it)->bad2);
    }

  }

    
    
  
    
};

typedef std::unordered_map<std::string, node> node_map_type;

#endif /* _NODE_H_ */
