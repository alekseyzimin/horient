#include <horient.hpp>
#include <set>
#include <iostream>


//Might not need these? Might be defined/referenced elsewhere?
//typedef std::list<edge>::iterator edge_ptr;
// struct edge_comparator {
//   int id;
//   edge_comparator(int id_) : id(id_) {}
//   inline int far_id(const edge_ptr& e){
//     return (*e->n1)->id = id ? (*e->n2)->id : (*e->n1)->id;
//   }
//   bool operator()(const edge_ptr& x, const edge_ptr& y){
//     return far_id(x) < far_id(y);
//   }
// };

template<typename T, typename U>
std::pair<T&, U&> tie(T& x, U& y) { return std::pair<T&, U&>(x, y); }

// Return a pair <good, bad> of the sum of the good and bad of each
// edge in the Container list. The edge e is skipped.
template<typename Container>
std::pair<int, int> sum_edges(const Container& list, const edge_ptr& e) {
  std::pair<int, int> res;

  for(auto it = list.begin(); it != list.end(); ++it) {
    if(comp_edge(*it, e))
      continue; //Skip the edge we are merging

    // 2 de-refences because iterator is to set of edge_ptrs, want edge-good
    res.first  += (*it)->good;
    res.second += (*it)->bad;
  }
  return res;
}

// This function picks the node we want to flip based on the good &
// bad weights on edges incident to the two nodes on a provided edge.
node& pick_flip(const edge_ptr& join_edge){
  int n1_good = 0;
  int n1_bad  = 0;
  int n2_good = 0;
  int n2_bad  = 0;

  // Sum up good and bad mate-pairs for all edges incident to Node 1
  // in edge(join_edge)

  //Sum up good and bad mate-pairs for all edges incident to Node 2 in edge(join_edge)
  tie(n1_good, n1_bad) = sum_edges(join_edge->n1.edges.local_list, join_edge);
  tie(n2_good, n2_bad) = sum_edges(join_edge->n2.edges.local_list, join_edge);

  //Decide if we should flip node 2
  int n1diff=n1_good-n1_bad;
  int n2diff=n2_good-n2_bad;

  // If more bad mates on node 2 and either no good on either, or
  // equal difference, flip 2.
  if( (n2_bad>n1_bad) && ( (n1_good == 0 && n2_good == 0 ) || (n1diff == n2diff) ) ) {
    return join_edge->n2;
  }
  // There are any good on node 1, and more satisified than
  // unsatisfied mates on 1, flip 2.
  if( n1diff > n2diff && n1_good != 0  ) {
    return join_edge->n2;
  }

  //If we don't decide to flip node 2, return that node 1 should be flipped.
  return join_edge->n1;
}
