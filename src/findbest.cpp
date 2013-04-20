#include <horient.hpp>
#include <list>
#include <cstdlib>


//It seems my indentation is off? Indicating I missed closing something somewhere??
// Not sure where, seems like in my original function name, but that seems to be ok??

typedef std::list<edge>::iterator edge_ptr;

//This function goes through the nodes connected to an edge, and finds what the merge
// loss would be. The merge loss is the bad (or good) edges subsumed in a merge.
//To Find merge loss:
//  Determine if either node would be flipped, if so which one.
//  Then determine the edges to mutual neighbors.
//  Determine if when merging edges to mutual neighbors any good/bad are "subsumed"
//      I.E. if AFTER FLIPPING, there are both (net) good and bad going to a neighbor.
//  Sum: ( (net) good + (net) bad ) - ( abs (  (net) good - (net) bad ) ) 
int findloss(edge_ptr e){

  int loss_cnt=0;
  node* node_to_flip=NULL; //To hold for flipping and unflipping.
  int diff_n1_edg, diff_n2_edg;//Good-bad difference on edges we find.

  if(e->good < e->bad) {node_to_flip = pick_flip( *e );}  //If we need to flip one, pick flip

  //Two options actually exist here, we can have a more complicated function which does NOT flip
  // the node before calculating. This will be far more efficient, but messier code.

  //We have initially implemented a flip then flip back model.
  if(node_to_flip != NULL){flip_node(node_to_flip);}

  //Iterate over internal edge lists to nodes connected to edge (e).
  // When find edge pointing to same neighbor, adjust loss-count

  //using auto to try and avoid a super deep reference to an iterator type...
  // These should be iterator which dereference to an edge_ptr. 
  auto n1__edg_it = e->n1->edges->local_list.begin();
  auto n2__edg_it = e->n2->edges->local_list.begin();
  

  //Not for loop because we increment independently.
  // should loop through the two sorted edge lists interior to the two nodes.
  while(n1_edg_it != e->n1->edges->local_list.end() && n2_edg_it != e->n2->edges->local_list.end() ){
    
    //If either iteration points to the edge we are merging on. Go to next loop
    if( *n1_edg_it == e) {n1_edg_it++;continue;}
    if( *n2_edg_it == e) {n2_edg_it++;continue;}

    //Check if nodes point to different edges.

    //Not same. N1's edge neighbor is less. Since in sorted, increment to next edge in N1
    if( e->n1.far_id(n1_edg_it) < e->n2.far_id(n2_edg_it) ) {n1_edg_it++;continue;}
    //Not same. N2's edge neighbor is less. Since in sorted, increment to next edge in N2 
    if( e->n1.far_id(n1_edg_it) > e->n2.far_id(n2_edg_it) ) {n2_edg_it++;continue;}

    //If we didn't loop yet, we must have Same edge. Confirm. 
    //(can be removed we we are satisfied rest is working right)
    if( e->n1.far_id(n1_edg_it) != e->n2.far_id(n2_edg_it) ) {cerr<<"something funky\n";exit(EXIT_FAILURE);}

    //We have same edge. So no we calculate options lost.
    diff_n1_edg= (*n1_edg_it)->good - (*n1_edg_it)->bad;
    diff_n2_edg= (*n2_edg_it)->good - (*n2_edg_it)->bad;

    if( (diff_n1_edg < 0 || diff_n2_edg < 0 ) && (diff_n1_edg > 0 || diff_n2_edg > 0 ) ){
      loss_cnt+=min(abs(diff_n1_edg), abs(diff_n2_edg) );
    }

  }



}

//We're going to be a little less efficient on this and iterate through all the edges,
// rather thans storing a maximum link per node, and iterating through the nodes. ( O(n) vs. O(e) ) 
edge* findbestmerge( std::list<edge>& master_edge){

//Want a list of iterators, into an edge list to indicate where in master_edge list max's are
std::list<edge_ptr> max_links;
double avg_score, max_score;

//Push the first edge so we have an initial "max" 
max_links.push_front(master_edge.cbegin());

//Actual max score is the edge score divided by the maximum possible links that could have been
// averaged into it, which is the size of the two nodes multiplied
max_score= (*master_edge.begin()).score()/(master_edge.begin()->n1->size *  master_edge.begin()->n2->size)

//Iterate through the entire master edge list, storing any edges with a maximum score.
for(edge_ptr m_edg_it = master_edge.begin(); m_edg_it != master_edge.end(); m_edg_it++) {

//The averaged score on a given node is the score, divided by the maximum possible edges in it.
avg_score= (*m_edg_it).score() / ( (*m_edg_it)->n1->size * (*m_edg_it)->n2->size );

//We didn't find a new maximum
if(avg_score < max_score){ continue;}

//If it's equal to our max score, push it onto the list of max-edges then go to next edge
if(avg_score==max_score){max_links.push_front(*m_edg_it); continue;}

//If we didn't continue above, we found a new max!
//Clear contents of list, and reset maxscore, and add new edge.
max_links.clear(); //clear contents
max_links.push_front(*m_edg_it); //push our new first max
max_score=avg_score; //make avg score our new max score.

}

  //We now have a list of max edges. If there is more than 1 we need to decide which of these to return
  if(max_links.size() == 1) {return *(max_links.cbegin());}  //Return contents of first element in max_links
    
    edge_ptr tmp_return; //Iterator in edge list, so points to an edge.
int min_merge_loss;

//if there is more than 1 element in the list we have to pick a join element.

tmp_return= *(max_links.begin()); //set our default return as the first edge
if( tmp_return->merge_loss!=NULL) { min_merge_loss = tmp_return->merge_loss; } //If loss set already, save it.
 else { min_merge_loss = findloss(*tmp_return); }  // If not set, find it and save it.

for(std::list<edge_ptr>::iterator max_it=max_links.begin(); max_it != max_links.end(); max_it++) {

//Iterate through list checking if there is a minimum merge loss



