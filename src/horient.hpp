#include <istream>
#include <list>

#include <edge_list.hpp>
#include <node.hpp>

typedef edge_base<node> edge;

void readdata(std::list<edge>& master_edge, node_map_type& master_node, bool filters, std::istream& input);
