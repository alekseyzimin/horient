#include <edge_list.hpp>
#include <list>
#include <iostream>

typedef disjoint_set<int> node;
typedef edge_base<node> edge;

//Assumes fntally has been defined globally.
//Assumes the tally-file is in a 6-column format
//Requires access to the master edge & node lists.
void readdata(list<edge>& master_edge, list<node>& master_node){
  ifstream tallyfile;
  int node1,node2,wght1,wght2,wght3,wght4;
  node* tmp_n0,tmp_n1;
  edge* tmp_e;

  tallyfile.open(fntally.c_string(),"r");
  if(tallyfile.is_open())
    {
      while(tallyfile.good())
	{
	  node1<<tallyfile;
	  node2<<tallyfile;
	  wght1<<tallyfile;
	  wght2<<tallyfile;
	  wght3<<tallyfile;
	  wght4<<tallyfile;

	  tmp_n0=master_node.find(node1);
	  tmp_n1=master_node.find(node2);
	  tmp_e=new edge(tmp_n0,tmp_n1,wght1,wght2);
	}
    }
  else{cout<<"Error in opening tally file"<<endl;exit(EXIT_FAILURE)}
  
  tallyfile.close();
}
