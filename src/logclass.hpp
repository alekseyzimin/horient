#ifndef _LOGCLASS_H_
#define _LOGCLASS_H_

#include <iostream>
#include <string>
#include <horient.hpp>



class logging {
public:
  virtual void log(node flippednode, int indx) = 0;

  virtual  void log(edge_ptr join_edge, int indx) = 0;

};


class log_out: public logging{
  std::ofstream flipout;//Outstream for flips
  std::ofstream joinout;//Outstream for joins

public:
  log_out(std::string a, std::string b) :
    flipout(a.c_str(), std::ofstream::out | std::ofstream::app),
    joinout(b.c_str(), std::ofstream::out | std::ofstream::app)
  {
    if(!flipout.good()){
      std::cerr<<"Fail to open flip log file '"<<a<<"'"
	       << std::endl;
      exit(EXIT_FAILURE);
    }

    if(!joinout.good()){
      std::cerr<<"Fail to open join log file '"<<b<<"'"
	       << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  void log(node flippednode, int indx){
    flipout<<"Flip node: "<<flippednode<<"\t Step: "<<indx<<std::endl;
  };

  void log(edge_ptr join_edge, int indx){
    joinout<<"Join Edge: "<<*join_edge<<"\t\t\t Step: "<<indx<<std::endl;
  };

};

//log_out::log_out(std::string a, std::string b):logging(){
//};

class log_null: public logging {
public:
  void log(node flippednode, int indx){ /*do nothing*/  }
  void log(edge_ptr join_edge, int indx){ /*do nothing*/}

};

#endif /* _LOGCLASS_H_ */
