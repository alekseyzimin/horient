#include <edge_list.hpp>
#include <list>
#include <iostream>

typedef disjoint_set<int> node;
typedef edge_base<node> edge;

//Assumes fntally has been defined globally.
//Assumes the tally-file is in a 6-column format
//Requires access to the master edge & node lists.
void readdata(list<edge>& master_edge, list<node>& master_node, bool filters){
  ifstream tallyfile;
  int node1,node2,wght1,wght2,wght3,wght4;
  node* tmp_n0,tmp_n1;
  edge* tmp_e;
  list<edge>::const_iterator edge_it;

  tallyfile.open(fntally.c_string(),"r");
  //If we didn't fail to open the file...
  if(tallyfile.is_open())
    {
      //While we haven't hit the end of file.
      while(tallyfile.good())
	{
	  //Read in the 6 values
	  node1<<tallyfile;
	  node2<<tallyfile;
	  wght1<<tallyfile;
	  wght2<<tallyfile;
	  wght3<<tallyfile;
	  wght4<<tallyfile;

	  //If we are using filters, and none of the conditions are true... try storing it
	  // Conditions are: 
	  //                satisified Mate-pairs == unsatisfied mate-pairs
	  //                only 1 mate-pair on edge
	  //                OR not using filters...
	  // Currently, if there is data in wght3/4 there will be problems with the filter.
	  if( !filters ||
	      (filters && 
	       !( (wght3==0 && wght4==0) && 
		  ( (wght1==1 && wght2==0) || (wght2==1 && wght1==0)|| (wght1==wght2) ) ) ) ) 
	    {
	      //Find (or create) pointers to the input nodes.
	      tmp_n0=master_node.find(node1);
	      tmp_n1=master_node.find(node2);
	      //Make a new edge
	      tmp_e=new edge(tmp_n0,tmp_n1,wght1,wght2);
	      
	      //Add the edge to the master list, and each node;
	      edge_it=master_edge.addedge(tmp_e);

	      //Use the returned iterator to add in nodes
	      (&tmp_n0).addedge(edge_it);
	      (&tmp_n1).addedge(edge_it);
	    }

	}
    }
  //If we failed to open the file, output error and exit.
  else{cout<<"Error in opening tally file"<<endl;exit(EXIT_FAILURE)}
  
  tallyfile.close();
}
