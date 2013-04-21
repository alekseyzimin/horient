#ifndef _FINDBEST_H_
#define _FINDBEST_H_

#include <horient.hpp>
#include <list>
#include <cstdlib>
#include <iostream>


//It seems my indentation is off? Indicating I missed closing something somewhere??
// Not sure where, seems like in my original function name, but that seems to be ok??

typedef std::list<edge>::iterator edge_ptr;



//We're going to be a little less efficient on this and iterate through all the edges,
// rather thans storing a maximum link per node, and iterating through the nodes. ( O(n) vs. O(e) ) 
edge_ptr findbestmerge( std::list<edge>& master_edge){

  //Want a list of iterators, into an edge list to indicate where in master_edge list max's are
  std::list<edge_ptr> max_links;
  double avg_score, max_score;

  //Push the first edge so we have an initial "max" 
  max_links.push_front(master_edge.begin());

  //Actual max score is the edge score divided by the maximum possible links that could have been
  // averaged into it, which is the size of the two nodes multiplied
  max_score= 
    (*master_edge.begin()).score() / 
    ( (*master_edge.begin()->n1)->size *  (*master_edge.begin()->n2)->size);
    
  //Iterate through the entire master edge list, storing any edges with a maximum score.
  for(edge_ptr m_edg_it = master_edge.begin(); m_edg_it != master_edge.end(); m_edg_it++) {

    //The averaged score on a given node is the score, divided by the maximum possible edges in it.
    avg_score= 
      (*m_edg_it).score() / 
      ( (*m_edg_it->n1)->size * (*m_edg_it->n2)->size );

    //We didn't find a new maximum, loop in for.
    if(avg_score < max_score){ continue;}

    //If it's equal to our max score, push it onto the list of max-edges then go to next edge
    if(avg_score==max_score){max_links.push_front(m_edg_it); continue;}

    //If we didn't continue above, we found a new max!
    //Clear contents of list, reset maxscore, and add new edge.
    max_links.clear(); //clear contents
    max_links.push_front(m_edg_it); //push our new first max
    max_score=avg_score; //make avg score our new max score.
    
  }

  //We now have a list of max edges. If there is only 1, return it.
  if(max_links.size() == 1) {return *(max_links.begin());}  //Return contents of first element in max_links
  
  //Otherwise we need we have more than 1 and need to decide which of these to return
  edge_ptr tmp_return; //Iterator in edge list, so points to an edge.
  int min_merge_loss;

  //if there is more than 1 element in the list we have to pick a join element.

  tmp_return= *(max_links.begin()); //set our default return as the first edge
  if( tmp_return->merge_loss >= 0) 
    { min_merge_loss = tmp_return->merge_loss; } //If loss set already, save it.
  else { min_merge_loss = find_loss(tmp_return); }  // If not set, find it and save it.

  for(std::list<edge_ptr>::iterator max_it=max_links.begin(); max_it != max_links.end(); max_it++) {

    //Iterate through list checking if there is a minimum merge loss
    
    //If the merge-loss is already known for the edge, check it. 
    if( (*max_it)->merge_loss >= 0 && (*max_it)->merge_loss < min_merge_loss ){
      tmp_return = *max_it;
      min_merge_loss = tmp_return->merge_loss;
      continue;
    }

    //Otherwise we need to Calculate the merge-loss THEN check it.
    // Note: the call to find_loss guarantees a set value in merge_loss....
    if( min_merge_loss > find_loss(*max_it) ) {
      tmp_return = *max_it; 
      min_merge_loss = tmp_return->merge_loss;
      continue; //This is sort of irrelevant but is for carefulness with later possible logic
    }

    //If they are equal... we should probably do something to decide more!
    if( min_merge_loss == (*max_it)->merge_loss ) { /*do nothing for now*/ };
  }

  //Sanity check.
  if(min_merge_loss < 0 ){ std::cerr<<"Something funky in find-best\n"; exit(EXIT_FAILURE);}

  return tmp_return;

}
      

    


#endif /* _FINDBEST_H_ */
