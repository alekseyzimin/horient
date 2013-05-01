#ifndef _LOGCLASS_H_
#define _LOGCLASS_H_

#include <iostream>
#include <string>
#include <horient.hpp>



class logging {
public:
  virtual void log(const node& flippednode, int indx) = 0;

  virtual  void log(const edge_ptr& join_edge, int indx) = 0;

};


class log_out: public logging{
  std::ofstream logout; //Outstream for flips

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

  void log(const node& flippednode, int indx){
    logout<<"Node flip:\t"<<flippednode<<"\tStep:\t"<<indx<<std::endl;
  };

  void log(const edge_ptr& join_edge, int indx){
    logout<<"Edge join:\t"<<*join_edge<<"\tStep:\t"<<indx<<std::endl;
  };

};

//log_out::log_out(std::string a, std::string b):logging(){
//};

class log_null: public logging {
public:
  void log(const node& flippednode, int indx){ /*do nothing*/  }
  void log(const edge_ptr& join_edge, int indx){ /*do nothing*/}

};

#endif /* _LOGCLASS_H_ */
