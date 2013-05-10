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


#include <istream>
#include <iostream>
#include <string>
#include <utility>

#include <horient.hpp>

node& insert_node(std::string label, node_map_type& master_node, int& id, std::string nam) {
  auto it = master_node.find(label);
  if(it != master_node.end())
    return it->second;
  return master_node.insert(std::make_pair(label, node(id++, nam))).first->second;
}

//Assumes fntally has been defined globally.
//Assumes the tally-file is in a 6-column format
//Requires access to the master edge & node lists.
void readdata(
std::list<edge>& master_edge, node_map_type& master_node, bool filters, std::istream& input) {
  std::string node1, node2;
  int         wght1,wght2,wght3,wght4;
  int         id = 0;

  //While we haven't hit the end of file.
  while(input.good()) {
    //Read in the 6 values
    input >> node1 >> node2
          >> wght1 >> wght2 >> wght3 >> wght4;
    if(!input.good())
      break;

    //If we are using filters, and none of the conditions are true... try storing it
    // Conditions are:
    //                satisified Mate-pairs == unsatisfied mate-pairs
    //                only 1 mate-pair on edge
    //                OR not using filters...
    // Currently, if there is data in wght3/4 there will be problems with the filter.
    if(filters && ( (wght1 == 1 && wght2 == 0) || (wght2 == 1 && wght1 == 0) || (wght1 == wght2) ) )
      continue;

    //Find (or create) pointers to the input nodes.
    node& tmp_n0 = insert_node(node1, master_node, id, node1);
    node& tmp_n1 = insert_node(node2, master_node, id, node2);

    //Zero out single 'bad' mate-pairs (if there is data in other entry)
    if(filters && wght1==1 && wght2>1) { wght1=0; }
    if(filters && wght2==1 && wght1>1) { wght2=0; }

    //Make a new edge
    master_edge.push_front(edge(tmp_n0, tmp_n1, wght1, wght2));

    //Check if edge is duplicated
    auto it = tmp_n0.find_edge(master_edge.begin());
    if(!it.first) { // New -> add to adjacent nodes
      tmp_n0.add_edge(master_edge.begin());
      tmp_n1.add_edge(master_edge.begin());
    } else {  // found -> update existing
      it.second->good  += wght1;
      it.second->bad   += wght2;
      // it.second->good2 += wgth3;
      // it.second->bad2  += wght4;
      master_edge.pop_front();
    }
  }
}
