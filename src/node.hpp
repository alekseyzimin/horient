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

  node_info() : id(-1), int_good(0), int_bad(0), size(1) { }
  void add_edge(edge_ptr e) { }
};

typedef std::unordered_map<std::string, node> node_map_type;

#endif /* _NODE_H_ */
