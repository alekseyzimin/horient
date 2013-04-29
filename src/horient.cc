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


#include <cstdlib>

#include <fstream>
#include <string>
#include <horient.hpp>
#include <src/horient_cmdline.hpp>
#include <logclass.hpp>

void output_contig_orientation(std::ostream& out, node_map_type& master_node) {
  for(auto it = master_node.begin(); it != master_node.end(); ++it) {
    out << it->first << " " << it->second.resolve_orientation() << "\n";
  }
}

int main(int argc, char *argv[])
{
  horient_cmdline args(argc, argv);

  std::list<edge>& master_edge = node::sorted_edge_list_type::master_list;
  node_map_type master_node;
  //std::ofstream outfile;
  logger * master_logger;
  int step_index=0;

  if(args.logflip_given && args.logjoin_given)
    { master_logger = new log_out(args.logflip_arg,args.logjoin_arg);   }
  else if(args.logflip_given)
    { master_logger = new log_out(args.logflip_arg,"/dev/null");}
  else if(args.logjoin_given)
    { master_logger = new log_out("/dev/null",args.logjoin_arg);} 
  else
    { master_logger = new log_null;}

	
  std::ifstream input(args.input_arg);
  if(!input.good()) {
    std::cerr << "Can't open input file '" << args.input_arg << "'" << std::endl;
    exit(EXIT_FAILURE);
  }
  readdata(master_edge, master_node, args.foff_flag, input);

  //While we have edges we haven't joined... loop
  edge_ptr join_edge;
  while(master_edge.size()>0) {
    // Pick the best edge to merge
    // NOTE: does not track if multiple "fewest options discarded" (FOD) edges are found.
    //      This means that there is still some ambiguity in join edge choice.
    //      This could be fixed by more levels of decisions on FOD. (recursive?)
    join_edge=findbestmerge(master_edge);

    //Perform flipping if necessary
    if(join_edge->good < join_edge->bad) {
      //If there are less satisfied than unsatisified mate-pairs on
      //the edge, flip a node
      pick_flip(join_edge).flip_node();
    }

    //Since we've flipped any necessary node. Now we can merge them.
    merge_nodes(join_edge->n1, join_edge->n2);

    ++step_index;
  }

  // We always have at least one component.
   int nb_connected_components = 1;

   if(args.statistics_given) {
    std::ofstream stats(args.statistics_arg);
    if(!stats.good()) {
      std::cerr << "Failed to open statistics file '" << args.statistics_arg << "'"
                << std::endl;
      return EXIT_FAILURE;
    }
    // Compute number of connected components
    --nb_connected_components;
    for(auto it = master_node.cbegin(); it != master_node.cend(); ++it) {
      nb_connected_components += (it->second.parent == 0);
    }
    stats << "Original Contig count: "<< master_node.size() << "\n"
          << "Connected Components:  "<< nb_connected_components << "\n";
  }

  if(args.output_given) {
    std::ofstream file(args.output_arg);
    if(!file.good()) {
      std::cerr << "Failed to open output file '" << args.output_arg << "'" << std::endl;
      return EXIT_FAILURE;
    }
    output_contig_orientation(file, master_node);
  } else {
    output_contig_orientation(std::cout, master_node);
  }

  //  int good_bad[nb_connected_components][2];

  return 0;
}
