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

  edge_base(N* x, N* y, int g, int b) : n1(x), n2(y), good(g), bad(b), good2(0), bad2(0) { }
  edge_base(N* x, N* y, int g, int b, int g2, int b2) :
    n1(x), n2(y),
    good(g), bad(b), good2(g2), bad2(b2)
  { }

  bool operator<(const edge_base& e) const {
    return *n1 < *e.n1 || (!(*e.n1 < *n1) && *n2 < *e.n2);
  }

  //  double score() const { S f; return f(good, bad, good2, bad2); }
  double score() const { return score_function(good, bad, good2, bad2); }
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
