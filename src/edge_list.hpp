#ifndef _EDGE_LIST_H_
#define _EDGE_LIST_H_

#include <list>
#include <set>

typedef double (*scoring_function)(int g, int b, int g2, int b2);

// Default scoring function
inline double diff_square(const int g, const int b, const int g2, const int b2) {
  double d = g - b;
  return d * d / (double)(g + b);
}


// Edge base class. N is node type, S is scoring function, taking 4
// ints.
template<typename N>
struct edge_base {
  static scoring_function score_function;
  N* n1;
  N* n2;
  // Good and bad are number of good and bad edges between nodes. Same
  // with good2 and bad2 with unreliable data. Not used right now.
  int good, bad;
  int good2, bad2;
  // merge_loss tracks how many bad are "lost" when this nodes on
  // either side are merged.  This should be cleared to '-1' every
  // time we merge either connected node!!
  int merge_loss; //default -1, indicating not calculated yet.

  edge_base(N* x, N* y, int g, int b) :
    n1(x), n2(y), good(g), bad(b), good2(0), bad2(0), merge_loss(-1)
  { }
  edge_base(N* x, N* y, int g, int b, int g2, int b2) :
    n1(x), n2(y),
    good(g), bad(b), good2(g2), bad2(b2), merge_loss(-1)
  { }

  bool operator<(const edge_base& e) const {
    return *n1 < *e.n1 || (!(*e.n1 < *n1) && *n2 < *e.n2);
  }

  //  double score() const { S f; return f(good, bad, good2, bad2); }
  double score() const { return score_function(good, bad, good2, bad2); }

  //This does not actually destroy the merged edge. It must be destroyed after calling.
  //Note: I don't know if this is actually generalized/templated nicely..or will break outside of this?
  void merge(const edge_base& e) {
    //Update interior values.
    good+= e.good;
    good2+= e.good2;
    bad+= e.bad;
    bad2+= e.bad2;
    merge_loss=-1;


    //If we pass in a edge_ptr we can use "comp_edge" ... alternatively, we could
    // change "comp_edge" to actually compare edges, instead of pointers, then pass
    // into it more derefenced things. Have to make those tiered changes,
    // This would all be the cleaner approach. Works for now. Can clean up later.

    //Because we have an edge_base / edge in... have to do this messier. 
    //  Maybe we can just pass a pointer to the actual edge? not sure if
    // guaranteed to be identical pointers as iterator would general.

    // Find edge in local list to erase..
    auto local_it = (*e.n1).value.edges.local_list.begin(); //just for readability.
    auto local_end= (*e.n1).value.edges.local_list.end();

    //Have to check id's because only place == or != defined! (for int's).
    //Loop through edge_ptrs until we find the one that points to our passed in edge.
    while(local_it != local_end){
      if( (* (*local_it)->n1)->id != (*e.n1)->id ) {++local_it;continue;}
      if( (* (*local_it)->n2)->id != (*e.n2)->id ) {++local_it;continue;}

      break;
    }

    //local_it should be an iterator to a pointer to same edge as passed in now. 
    assert( (    (* (*local_it)->n1)->id == (*e.n1)->id )  
	    && ( (* (*local_it)->n2)->id == (*e.n2)->id ) ) ;
    
    (*e.n1).value.edges.local_list.erase(local_it);

    //Have to do all of above 2nd time for n2. Just to be careful!
    local_it = (*e.n2).value.edges.local_list.begin(); //just for readability.
    local_end= (*e.n2).value.edges.local_list.end();

    while(local_it != local_end){
      if( (* (*local_it)->n1)->id != (*e.n1)->id ) {++local_it;continue;}
      if( (* (*local_it)->n2)->id != (*e.n2)->id ) {++local_it;continue;}

      break;
    }
    //local_it should be an iterator to a pointer to same edge as passed in now. 
    assert( (    (* (*local_it)->n1)->id == (*e.n1)->id )  
	    && ( (* (*local_it)->n2)->id == (*e.n2)->id ) ) ;

    (*e.n2).value.edges.local_list.erase(local_it);
    
  }	  

    
};
template<typename N>
scoring_function edge_base<N>::score_function = diff_square;

// Keep a sorted list of edge. The sorting key is the far end of the
// edge with respect to the current node, as passed to add_edge (id
// parameter). In effect it is a sorted neighbor list.
template<typename E>
struct sorted_edge_list {
  typedef typename std::list<E>::iterator edge_ptr;
  struct edge_comparator {
    int id;
    edge_comparator(int id_) : id(id_) { }
    inline int far_id(const edge_ptr& e) {
      return (*e->n1)->id == id ? (*e->n2)->id : (*e->n1)->id;
    }
    bool operator()(const edge_ptr& x, const edge_ptr& y) {
      return far_id(x) < far_id(y);
    }
  };

  static std::list<E>* master_list;
  std::set<edge_ptr, edge_comparator>  local_list;


  sorted_edge_list(int id) : local_list(edge_comparator(id)) { }

  void add_edge(const edge_ptr& e) { local_list.insert(e); }
  void merge(sorted_edge_list& rhs) { }
};

#endif /* _EDGE_LIST_H_ */
