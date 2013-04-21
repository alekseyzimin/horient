#include <cstdlib>

#include <fstream>

#include <horient.hpp>
#include <src/horient_cmdline.hpp>


int main(int argc, char *argv[])
{
  horient_cmdline args(argc, argv);

  std::list<edge> master_edge;
  node_map_type master_node;

  std::ifstream input(args.input_arg);
  if(!input.good()) {
    std::cerr << "Can't open input file '" << args.input_arg << "'" << std::endl;
    exit(EXIT_FAILURE);
  }
  readdata(master_edge, master_node, args.foff_flag, input);

  return 0;
}
