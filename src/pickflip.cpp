#include<horient.hpp>

//This function picks the node we want to flip based on the good & bad weights on edges incident to 
// the two nodes on a provided edge.
node* pickflip(edge join_edge){
  int n1_good=0;
  int n1_bad=0;
  int n2_good=0;
  int n2_bad=0;

  //Sum up good and bad mate-pairs for all edges incident to Node 1 in edge(join_edge)
  for(std::set<edge_ptr,edgecomparator>::iterator it= join_edge->n1->edges->local_list.begin(); it!=join_edge->n1->edges->local_list.end(); it++)
    {
      if( *it == join_edge ){continue;} //Skip the edge we are merging because of
      n1_good+= ( *(*it))->good;
      n1_bad+=  ( *(*it))->bad;
    }

  //Sum up good and bad mate-pairs for all edges incident to Node 2 in edge(join_edge)			       
  for(std::set<edge_ptr,edgecomparator>::iterator it= join_edge->n2->edges->local_list.begin(); it!=join_edge->n2->edges->local_list.end(); it++)
    {
      if( *it == join_edge ){continue;}  //Skip the edge we are merging because of

      n2_good+= ( *(*it))->good;
      n2_bad+=  ( *(*it))->bad;
    }

  //Decide if we should flip node 2
  int n1diff=n1_good-n1_bad;
  int n2diff=n2_good-n2_bad;

  //If more bad mates on node 2 and either no good on either, or equal difference, flip 2.
  if( (n2_bad>n1_bad) && ( (n1_good == 0 && n2_good == 0 ) || (n1diff == n2diff) ) )   { return join_edge->n2; }
  //There are any good on node 1, and more satisified than unsatisfied mates on 1, flip 2.
  if( n1diff > n2diff && n1_good != 0  ) { return join_edge->n2; }

  //If we don't decide to flip node 2, return that node 1 should be flipped.
  return join_edge->n1;
}
