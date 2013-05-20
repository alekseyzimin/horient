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

  virtual  void log(const edge_ptr& join_edge) = 0;

  virtual void log(const node* child, int indx)=0;

  virtual ~logging() { }
};


class log_out: public logging{
  std::ofstream logout; //Outstream for flips
  //  std::string tmpbuff;
  node* tmpflp=NULL; //Hold a pointer to flipped node for checking output.
  int tmpgood=0;
  int tmpbad=0;


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

  ~log_out(){logout.close();}

  void log(node& flippednode){
    tmpflp=&flippednode; //Just store which node got flipped.
  };

  void log(const edge_ptr& join_edge){

    //Store some information about the join.
    tmpgood=join_edge->good;
    tmpbad= join_edge->bad;
 };

  void log(const node* child, int indx){

    //Log the two nodes we joined.
    logout<<child->parent->name<<"\t"<<child->name;

    //Log the flips if any.
    if(tmpflp==child){ logout<<"\t+-\t";}    //If we previously recorded a flip on the child...
    else if(tmpflp==child->parent){logout<<"\t-+\t";  } //If we recorded flip on parent...
    else{ logout<<"\t++\t";}  //If we didn't record flip

    tmpflp=NULL; //Reset node pointer. Otherwise error on flip logic above.
   

    logout<<"good\t"<<tmpgood<<"\tbad\t"<<tmpbad	
	  <<"\tStep\t"<<indx<<"\n";

  }

};

//log_out::log_out(std::string a, std::string b):logging(){
//};

class log_null: public logging {
public:
  void log(node& flippednode){ /*do nothing*/  }
  void log(const edge_ptr& join_edge){ /*do nothing*/}
  void log(const node* child,  int indx){ /*do nothing*/}

};

#endif /* _LOGCLASS_H_ */
