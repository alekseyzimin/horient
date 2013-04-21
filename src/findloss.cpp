#ifndef _FINDLOSS_H_
#define _FINDLOSS_H_

#include <horient.hpp>
#include <cstdlib>
#include <algorithm>
#include <iostream>

typedef std::list<edge>::iterator edge_ptr;


//This function goes through the nodes connected to an edge, and finds what the merge
// loss would be. The merge loss is the bad (or good) edges subsumed in a merge.
//To Find merge loss:
//  Determine if either node would be flipped, if so which one.
//  Then determine the edges to mutual neighbors.
//  Determine if when merging edges to mutual neighbors any good/bad are "subsumed"
//      I.E. if AFTER FLIPPING, there are both (net) good and bad going to a neighbor.
//  Sum: ( (net) good + (net) bad ) - ( abs (  (net) good - (net) bad ) ) 
int find_loss(edge_ptr e){

  int loss_cnt=0;
  node* node_to_flip=NULL; //To hold for flipping and unflipping.
  int diff_n1_edg, diff_n2_edg;//Good-bad difference on edges we find.

  if(e->good < e->bad) {node_to_flip = pick_flip( e );}  //If we need to flip one, pick flip

  //Two options actually exist here, we can have a more complicated function which does NOT flip
  // the node before calculating. This will be far more efficient, but messier code.

  //We have initially implemented a flip then flip back model.
  if(node_to_flip != NULL){flip_node(node_to_flip);}

  //Iterate over internal edge lists to nodes connected to edge (e).
  // When find edge pointing to same neighbor, adjust loss-count

  //using auto to try and avoid a super deep reference to an iterator type...
  // These should be iterator which dereference to an edge_ptr. 
  auto n1_edg_it = (*e->n1)->edges.local_list.begin();
  auto n2_edg_it = (*e->n2)->edges.local_list.begin();
  

  //Not for loop because we increment independently.
  // should loop through the two sorted edge lists interior to the two nodes.
  while(n1_edg_it != (*e->n1)->edges.local_list.end() && n2_edg_it != (*e->n2)->edges.local_list.end() ){
    
    //If either iteration points to the edge we are merging on. Go to next loop
    if( *n1_edg_it == e) {n1_edg_it++;continue;}
    if( *n2_edg_it == e) {n2_edg_it++;continue;}

    //Check if nodes point to different edges.

    //Not same. N1's edge neighbor is less. Since in sorted, increment to next edge in N1
    if( (*e->n1)->far_id(*n1_edg_it) < (*e->n2)->far_id(*n2_edg_it) ) {n1_edg_it++;continue;}
    //Not same. N2's edge neighbor is less. Since in sorted, increment to next edge in N2 
    if( (*e->n1)->far_id(*n1_edg_it) > (*e->n2)->far_id(*n2_edg_it) ) {n2_edg_it++;continue;}

    //If we didn't loop yet, we must have Same edge. Confirm. 
    //(can be removed we we are satisfied rest is working right)
    if( (*e->n1)->far_id(*n1_edg_it) != (*e->n2)->far_id(*n2_edg_it) ) 
      {std::cerr<<"something funky\n";exit(EXIT_FAILURE);}

    //We have same edge. So now we calculate options lost.
    diff_n1_edg= (*n1_edg_it)->good - (*n1_edg_it)->bad;  //Get differences on edge from n1
    diff_n2_edg= (*n2_edg_it)->good - (*n2_edg_it)->bad;  //Get differences on edge from n2

    //Checking that one diff is <0 and one diff >0 means one edge is mostly 'good' and one mostly 'bad'
    if( (diff_n1_edg < 0 || diff_n2_edg < 0 ) && (diff_n1_edg > 0 || diff_n2_edg > 0 ) ){
      loss_cnt+=std::min(std::abs(diff_n1_edg), std::abs(diff_n2_edg) ); //loss count always positive or 0
    }

    //We've increased expected merge loss from found edge to shared neighbor. Now we increment either 
    // iterator to move on to find NEXT shared neighbor 
    n1_edg_it++;

  }

  //Before returning, we have to unflip the node, if we flipped one.
  if(node_to_flip != NULL) {flip_node(node_to_flip);}

  //Set the merge_loss in the edge we computed for.
  e->merge_loss=loss_cnt;

  //Return the calculated merge_loss
  return loss_cnt;

}

#endif /* _FINDLOSS_H_ */
