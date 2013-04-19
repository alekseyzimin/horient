#ifndef _NODE_H_
#define _NODE_H_

#include <unordered_map>
#include <disjoint_set.hpp>
#include <edge_list.hpp>

struct node_info;
typedef disjoint_set<node_info> node;

struct node_info {
  int id;
  int int_good, int_bad;
  int size;
  typedef sorted_edge_list<edge_base<node> >::edge_ptr edge_ptr;
  sorted_edge_list<edge_base<node> > edges;

  node_info(int id_) : id(id_), int_good(0), int_bad(0), size(1), edges(id_) { }
  void add_edge(const edge_ptr& e) { edges.add_edge(e); }
};

typedef std::unordered_map<std::string, node> node_map_type;

#endif /* _NODE_H_ */
