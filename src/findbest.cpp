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


#ifndef _FINDBEST_H_
#define _FINDBEST_H_

#include <cstdlib>
#include <cassert>

#include <list>
#include <iostream>
#include <limits>

#include <horient.hpp>

//We're going to be a little less efficient on this and iterate through all the edges,
// rather thans storing a maximum link per node, and iterating through the nodes. ( O(n) vs. O(e) )
edge_ptr findbestmerge(std::list<edge>& master_edge){
  // Want a list of iterators, into an edge list to indicate where in
  // master_edge list max's are
  std::list<edge_ptr> max_links;
  double max_score = std::numeric_limits<double>::lowest();

  // Iterate through the entire master edge list, storing any edges
  // with a maximum score.
  for(edge_ptr m_edg_it = master_edge.begin(); m_edg_it != master_edge.end(); ++m_edg_it) {
    // The averaged score on a given node is the score, divided by the
    // maximum possible edges in it.
    double avg_score=
      m_edg_it->score() /
      ( m_edg_it->n1.size * m_edg_it->n2.size );

    // We didn't find a new maximum, loop in for.
    if(avg_score < max_score){ continue;}

    // If it's equal to our max score, push it onto the list of
    // max-edges then go to next edge
    if(avg_score == max_score) {
      max_links.push_front(m_edg_it);
      continue;
    }

    // If we didn't continue above, we found a new max!
    // Clear contents of list, reset maxscore, and add new edge.
    max_links.clear(); //clear contents
    max_links.push_front(m_edg_it); //push our new first max
    max_score = avg_score; //make avg score our new max score.
  }

  // We now have a list of max edges. If there is only 1, return it.
  if(max_links.size() == 1) {
    return max_links.front();
  }

  // Otherwise we need we have more than 1 and need to decide which of
  // these to return
  edge_ptr res;                 //Iterator in edge list, so points to an edge.
  int      min_merge_loss = std::numeric_limits<int>::max();

  //Iterate through list checking if there is a minimum merge loss
  for(auto max_it=max_links.begin(); max_it != max_links.end(); ++max_it) {
    if(find_loss(*max_it) < min_merge_loss) {
      res            = *max_it;
      min_merge_loss = res->merge_loss;
      continue;
    }

    //If they are equal... we should probably do something to decide more!
    // if( min_merge_loss == (*max_it)->merge_loss ) { /*do nothing for now*/ };
  }

  //Sanity check.
  assert(min_merge_loss >= 0);
  //  if(min_merge_loss < 0 ){ std::cerr<<"Something funky in find-best\n"; exit(EXIT_FAILURE);}

  return res;

}


#endif /* _FINDBEST_H_ */
