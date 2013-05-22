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
#include <memory>
#include <map>
#include <vector>

#include <horient.hpp>
#include <src/horient_cmdline.hpp>
#include <logclass.hpp>

struct components_statistics {
  std::map<node*, std::vector<std::pair<std::string, node*> > > components;
  int total_good, total_bad;

  components_statistics() : total_good(0), total_bad(0) { }
};

components_statistics output_stats_contig_orientation(std::ostream& out, node_map_type& master_node) {
  components_statistics res;

  for(auto it = master_node.begin(); it != master_node.end(); ++it) {
    node& n = it->second;
    out << it->first << " " << n.resolve_orientation() << "\n";
    if(!n.parent) { // Is root of component
      res.total_good += n.int_good;
      res.total_bad  += n.int_bad;
      res.components[&n].push_back(std::make_pair(it->first, &n));
    } else {
      res.components[n.parent].push_back(std::make_pair(it->first, &n));
    }
  }

  return res;
}

int main(int argc, char *argv[])
{
  horient_cmdline args(argc, argv);

  master_list_type& master_edge = node::sorted_edge_list_type::master_list;
  node_map_type master_node;
  //std::ofstream outfile;
  std::unique_ptr<logging> master_logger;
  int step_index=0;

  if(args.log_given)
    { master_logger.reset(new log_out(args.log_arg)); }
  else
    { master_logger.reset(new log_null);}

  std::ifstream input(args.input_arg);
  if(!input.good()) {
    std::cerr << "Can't open input file '" << args.input_arg << "'" << std::endl;
    exit(EXIT_FAILURE);
  }

  //Passing the negative of "foff" so that if flag is set, we turn filters off...
  readdata(master_edge, master_node, args.foff_flag, input);
  int nb_edges = master_edge.size();

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
      master_logger->log(pick_flip(join_edge).flip_node());
    }

    //Since we've flipped any necessary node. Now we can merge them.
    master_logger->log(join_edge);
    master_logger->log(merge_nodes(join_edge->n1, join_edge->n2), step_index);


    ++step_index;
  }

  components_statistics stats;
  if(args.output_given) {
    std::ofstream file(args.output_arg);
    if(!file.good()) {
      std::cerr << "Failed to open output file '" << args.output_arg << "'" << std::endl;
      return EXIT_FAILURE;
    }
    stats = output_stats_contig_orientation(file, master_node);
  } else {
    stats = output_stats_contig_orientation(std::cout, master_node);
  }
  //std::cout<<step_index<<"\n";

  if(args.statistics_given) {
    std::ofstream file(args.statistics_arg);
    file << "Contig count: " << master_node.size() << "\n"
         << "Edge count: " << nb_edges << "\n"
         << "Connected components: " << stats.components.size() << "\n"
         << "Total good: " << stats.total_good << "\n"
         << "Total bad: " << stats.total_bad << "\n";
    if(args.component_flag) {
      for(auto it = stats.components.cbegin(); it != stats.components.cend(); ++it) {
        file << "Component good: " << it->first->int_good << " bad: " << it->first->int_bad << " elements:";
        for(auto it2 = it->second.cbegin(); it2 != it->second.cend(); ++it2) {
          file << " " << it2->first << "(" << (it2->second->orient == 1 ? "+" : "-") << ")";
        }
        file << "\n";
      }
    }
  }

  return 0;
}
