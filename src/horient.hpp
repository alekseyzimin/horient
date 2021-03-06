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

#ifndef _HORIENT_H_
#define _HORIENT_H_

#include <istream>
#include <list>

#include <edge_list.hpp>
#include <node.hpp>

typedef edge_base<node> edge;
typedef std::set<edge, edge_score_comparator<edge> > master_list_type;
typedef master_list_type::iterator edge_ptr;

//Function to read data into master edge and master node list.
// filters -- indicates if simple data cleaning should occur
// PRECONDITION: 'input' has been successfully opened
//                input accesses a validly formated tally-file (6-columns)
// POSTCONDITION: master_edge !empty
// POSTCONDITION: master_node !empty
void readdata(master_list_type& master_edge, node_map_type& master_node, bool filters, std::istream& input);

//Function to read input file and lock the orienations of nodes
//PRECONDITION: 'input' has been successfully opened, file is list of node-labels
//POSTCONDITION: all nodes in input file in master_node has "flippable" set to false
void locknodes(node_map_type& master_node, std::istream& input);

//Function to pick a node to flip
//PRECONDITION: we have decided to flip a node connected to the passed edge
//POSTCONDITION: we return a pointer to a node we should flip (that is connected to the edge)
node& pick_flip(const edge_ptr& join_edge);

//Function to change orientation of node, and all respective data.
//PRECONDITION: node-n0 exists, and has edges connected to it.
//POSTCONDITION: if orient = 1  -> orient = -1 :  if orient = -1 -> orient = 1
//POSTCONDITION: All edges connected to node have swapped good/bad values. 

// Implemented inside "node information" struct now.
//void flip_node(node* n0);



//Function to determine the best edge to merge over.
//PRECONDITION: master_edge !empty
//POSTCONDITION: edge_ptr returned. 
//??POSTMAYBE: some edges have a merge-loss calculated.
edge_ptr findbestmerge(master_list_type& master_edge);

//Function to find the loss for merging over a given edge
//PRECONDITION: edge e exists, nodes connected exist, e->merge_loss == NULL 
//POSTCONDITION: e->merge_loss is set.  merge_loss also returned.
//POSTCONDITION: nodes connected to 'e' are returned to original state.
int find_loss(edge_ptr& e);

//Dummy function for now.
void flip_node(node* n1);

#endif /* _HORIENT_H_ */
