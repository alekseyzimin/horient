#ifndef _HORIENT_H_
#define _HORIENT_H_

#include <istream>
#include <list>

#include <edge_list.hpp>
#include <node.hpp>

typedef edge_base<node> edge;
typedef std::list<edge>::iterator edge_ptr;

//Function to read data into master edge and master node list.
// filters -- indicates if simple data cleaning should occur
// PRECONDITION: 'input' has been successfully opened
//                input accesses a validly formated tally-file (6-columns)
// POSTCONDITION: master_edge !empty
// POSTCONDITION: master_node !empty
void readdata(std::list<edge>& master_edge, node_map_type& master_node, bool filters, std::istream& input);

//Function to pick a node to flip
//PRECONDITION: we have decided to flip a node connected to the passed edge
//POSTCONDITION: we return a pointer to a node we should flip (that is connected to the edge)
node* pick_flip(edge_ptr join_edge);

//Function to change orientation of node, and all respective data.
//PRECONDITION: node-n0 exists, and has edges connected to it.
//POSTCONDITION: if orient = 1  -> orient = -1 :  if orient = -1 -> orient = 1
//POSTCONDITION: All edges connected to node have swapped good/bad values. 
void flip_node(node* n0);

//Function to determine the best edge to merge over.
//PRECONDITION: master_edge !empty
//POSTCONDITION: edge_ptr returned. 
//??POSTMAYBE: some edges have a merge-loss calculated.
edge_ptr findbestmerge(std::list<edge>& master_edge);

//Function to find the loss for merging over a given edge
//PRECONDITION: edge e exists, nodes connected exist, e->merge_loss == NULL 
//POSTCONDITION: e->merge_loss is set.  merge_loss also returned.
//POSTCONDITION: nodes connected to 'e' are returned to original state.
int find_loss(edge_ptr e);

//Function to set if two edges are the same from pointers to them. (iterators)
bool comp_edge(edge_ptr x, edge_ptr y);


//Dummy function for now.
void flip_node(node* n1);

#endif /* _HORIENT_H_ */
