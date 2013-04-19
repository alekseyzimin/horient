#include <istream>
#include <iostream>
#include <string>

#include <horient.hpp>


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
    node& tmp_n0 = master_node[node1];
    node& tmp_n1 = master_node[node2];
    if(tmp_n0->id < 0)
      tmp_n0->id = id++;
    if(tmp_n1->id < 0)
      tmp_n1->id = id++;

    //Zero out single 'bad' mate-pairs (if there is data in other entry)
    if(filters && wght1==1 && wght2>1) { wght1=0; }
    if(filters && wght2==1 && wght1>1) { wght2=0; }

    //Make a new edge
    master_edge.push_front(edge(&tmp_n0, &tmp_n1, wght1, wght2));

    //Use the returned iterator to add in nodes
    tmp_n0->add_edge(master_edge.begin());
    tmp_n1->add_edge(master_edge.begin());
  }
}
