/*  This file is part of Horient.

    Horient is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Horient is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Horient.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef _EDGE_LIST_H_
#define _EDGE_LIST_H_

#include <list>
#include <set>
#include <iostream>

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
  N& n1;
  N& n2;

  // Good and bad are number of good and bad edges between nodes. Same
  // with good2 and bad2 with unreliable data. Not used right now.
  int good, bad;
  int good2, bad2;
  // merge_loss tracks how many bad are "lost" when this nodes on
  // either side are merged.  This should be cleared to '-1' every
  // time we merge either connected node!!
  int merge_loss; //default -1, indicating not calculated yet.

  edge_base(N& x, N& y, int g, int b) :
    n1(x), n2(y), good(g), bad(b), good2(0), bad2(0), merge_loss(-1)
  { }
  edge_base(N& x, N& y, int g, int b, int g2, int b2) :
    n1(x), n2(y),
    good(g), bad(b), good2(g2), bad2(b2), merge_loss(-1)
  { }

  bool operator<(const edge_base& e) const {
    return n1 < e.n1 || (!(e.n1 < n1) && n2 < e.n2);
  }

  // Return node of edge not equal to n
  N& far_node(const N& n) {
    return n.id == n1.id ? n2 : n1;
  }

  N& local_node(const N& n) {
    return n.id == n1.id ? n1 : n2;
  }

  // N& local_node(const int id) {
  //   return id == n1->id ? n1 : n2;
  // }

  //  double score() const { S f; return f(good, bad, good2, bad2); }
  double score() const { return score_function(good, bad, good2, bad2); }
};

template<typename N>
std::ostream& operator<<(std::ostream& o, const edge_base<N>& e) {
  return o << "<id1:" << e.n1.name << " id2:" << e.n2.name
           << " good:" << e.good << " bad:" << e.bad << ">";
}

//Function to set if two edges are the same from pointers to them. (iterators)
template<typename T>
bool comp_edge(const T& x, const T& y) {
  return (( x->n1.id == y->n2.id ) && ( x->n2.id == y->n1.id ) ) ||
    (( x->n2.id == y->n2.id ) && ( x->n1.id == y->n1.id ));
}


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
      return e->n1.id == id ? e->n2.id : e->n1.id;
    }
    bool operator()(const edge_ptr& x, const edge_ptr& y) {
      return far_id(x) < far_id(y);
    }
  };

  static std::list<E> master_list;
  std::set<edge_ptr, edge_comparator>  local_list;


  sorted_edge_list(int id) : local_list(edge_comparator(id)) { }

  void add_edge(const edge_ptr& e) { local_list.insert(e); }
  std::pair<bool, edge_ptr> find_edge(const edge_ptr& e) {
    auto it = local_list.find(e);
    bool found = (it != local_list.end());
    return std::make_pair(found, found ? *it : e);
  }
  void merge(sorted_edge_list& rhs) { }
};
template<typename E>
std::list<E> sorted_edge_list<E>::master_list;

#endif /* _EDGE_LIST_H_ */
