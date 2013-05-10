#ifndef _LOGCLASS_H_
#define _LOGCLASS_H_

#include <iostream>
#include <string>
#include <horient.hpp>


/*
This function implements logging.
Log output takes the format:
<parent:name> <child:name> <orientation change> good <edge-good> bad <edge-bad> Step <step index>

orientation change is formatted:
+ = no change
- = change
So:
++ = no changes
+- = child changed
-+ = parent changed

*/


class logging {
public:
  virtual void log(node& flippednode) = 0;

  virtual  void log(const edge_ptr& join_edge, const node* parnt, int indx) = 0;

};


class log_out: public logging{
  std::ofstream logout; //Outstream for flips
  //  std::string tmpbuff;
  node* tmpflp=NULL; //Hold a pointer to flipped node for checking output.

public:
  log_out(std::string a) :
    logout(a.c_str())
  {
    if(!logout.good()){
      std::cerr<<"Fail to open flip log file '"<<a<<"'"
	       << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  void log(node& flippednode){
    tmpflp=&flippednode; //Just store which node got flipped.
  };

  void log(const edge_ptr& join_edge, const node* child, int indx){

    //Log the two nodes we joined.
    logout<<child->parent->name<<"\t"<<child->name;

    //Log the flips if any.
    if(tmpflp==child){ logout<<"\t+-\t";}    //If we previously recorded a flip on the child...
    else if(tmpflp==child->parent){logout<<"\t-+\t";  } //If we recorded flip on parent...
    else{ logout<<"\t++\t";}  //If we didn't record flip
    
    //Log some information about the join.
    logout<<"good\t"<<join_edge->good<<"\tbad\t"<<join_edge->bad	\
	  <<"\tStep\t"<<indx<<"\n";

    tmpflp=NULL; //Reset node pointer. Otherwise error on flip logic above.
  };

};

//log_out::log_out(std::string a, std::string b):logging(){
//};

class log_null: public logging {
public:
  void log(node& flippednode){ /*do nothing*/  }
  void log(const edge_ptr& join_edge, const node* parnt, int indx){ /*do nothing*/}

};

#endif /* _LOGCLASS_H_ */
