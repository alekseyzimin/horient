#include <lots of stuff>
#include <list>
#include <string>

typedef disjoint_set<int> node;
typedef edge_base<node> edge;


string outlbl="outort"; //Default output label
string fntally="orientations_tallies"; // Default input file
double alpha=0.0; // Default weighting for column 5/6

int main(int argc, const char **argv)
{
  list<node> master_node;
  list<edge> master_edge;
  bool filters=true; //Use basic filtering? Default-true.
  list<edge>::iterator join_edge;
  list<node>::iterator tmp_node;
  bool flip;

  //Autogenerate argument read how?

  //Argument options should be:
  //
  // --i <c-string> :: input file name
  // --o <c-string> :: output file name
  // --foff :: turn 'filters->false'
  // --w <double> :: weighting for columns 5/6  -- DOES NOT NEED TO BE IMPLEMENTED IMMEDIATELY
  // --m <int or char?> :: this is to allow options for choice method. NOT IMPLEMENTED.

  readdata(master_edge,master_node,filters);

  int total_nodes=master_node.size();
  int orient[total_nodes]=1; //Initialize all orientations to 1.

  //While we have edges we haven't joined... loop
  while(master_edge.size()>0)
    {
      //Pick the best edge to merge (this is complex!)
      join_edge=findbestmerge(master_edge,master_node);
      
      //Perform flipping if necessary
      if((*join_edge).good<(*join_edge).bad) 
	//If there are less satisfied than unsatisified mate-pairs on the edge, flip a node.
	{
	  tmp_node=pick_flip(master_edge,master_node,join_edge); //Hides several if statements to pick a flip.

	  (*tmp_node).flip(orient); //Flip the orientations for that node.
	}

      //Since we've flipped any necessary node. Now we can merge them.
      (*(*join_edge).n1).merge((*join_edge).n2);
    }

  //Output the orientations
  ofstream ofile;
  ofile.open(outlbl.c_string());

  //Output the original contig name/number and it's determined orientation.
  if(ofile.is_open())
    {
      for(int ii=0;ii<total_nodes;ii++)
	{ ofile<<master_node.name(ii)<<"\t"<<orient[ii]<<endl;  }
    }
  else { cout<<"Error to open output file 1\n"; exit(EXIT_FAILURE);}

  ofile.close();

  string tmpname=outlbl+"_statistics";
  ofile.open(tmpname);

  if(ofile.is_open())
    {
      ofile<<"Original Contig count: "<<total_nodes<<endl;
      ofile<<"Connected Components:  "<<master_nodes.size()<<endl;
      //Output the good/bad count for each component
      //output the size of each component

      //output the sum of good/bad for all components
      //If we track them, output the number of times we could not determine a "best" merge.
    }

  ofile.close();
}
	  
