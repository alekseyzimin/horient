#include <iostream>
#include <string>
#include <horient.hpp>



class logging {
public:
  logging(){}
  virtual void log(node flippednode, int indx){  };

  virtual  void log(edge_ptr join_edge, int indx){}; 

};


class log_out: public logging{
  std::ofstream flipout;//Outstream for flips
  std::ofstream joinout;//Outstream for joins

public:
  log_out(std::string a, std::string b):logging(){
    flipout.open(a.c_str(), std::ofstream::out | std::ofstream::app);
    if(!flipout.good()){
      std::cerr<<"Fail to open flip log file '"<<a<<"'"
	       << std::endl;
      exit(EXIT_FAILURE);
    }
    
    joinout.open(b.c_str(), std::ofstream::out | std::ofstream::app);
    
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

class log_null: public logging{
public:
  void log(node flippednode, int indx){ /*do nothing*/  }
  
  void log(edge_ptr join_edge, int indx){ /*do nothing*/}
  
};
