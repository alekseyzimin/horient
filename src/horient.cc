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

#include <horient.hpp>
#include <src/horient_cmdline.hpp>


int main(int argc, char *argv[])
{
  horient_cmdline args(argc, argv);

  std::list<edge>& master_edge = node::sorted_edge_list_type::master_list;
  node_map_type master_node;

  std::ifstream input(args.input_arg);
  if(!input.good()) {
    std::cerr << "Can't open input file '" << args.input_arg << "'" << std::endl;
    exit(EXIT_FAILURE);
  }
  readdata(master_edge, master_node, args.foff_flag, input);

  return 0;
}
