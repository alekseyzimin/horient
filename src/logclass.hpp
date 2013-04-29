#include <iostream>
#include <string>
#include <horient.hpp>

/*struct nullstream: std::ostream{
  struct nullbuf: std::streambuf{
    int overflow(int c) { return traits_type::not_eof(c);}
    m_sbuf;
    nullstream():std::ios(&m_sbuf),std::ostream(&m_sbuf){}
  };
  };*/

class logging {
public:
  logging(){};
  virtual void log(node flippednode, int indx){  };

  virtual  void log(edge_ptr join_edge, int indx){}; 

};

class log_out: public logging{
  std::ostream flipout;//Outstream for flips
  std::ostream joinout;//Outstream for joins

public:
  log_out (string, string);
  
  void log(node flippednode, int indx){
    /*do stuff*/

  };

  void log(edge_ptr join_edge, int indx){
    /*do stuff!*/
  };

};

log_out::log_out ( string a, string b){
  logging();
  flipout.open(a.c_string());
  if(!flipout.good()){
    std::cerr<<"Fail to open flip log file '"<<a<<"'"
		 << std::endl;
	return EXIT_FAILURE;
  }

  joinout.open(b.c_string());

  if(!joinout.good()){
    std::cerr<<"Fail to open join log file '"<<b<<"'"
		 << std::endl;
	return EXIT_FAILURE;
  }

}
class log_null: public logging{
public:
  void log(node flippednode, int indx){ /*do nothing*/  }
  
  void log(edge_ptr join_edge, int indx){ /*do nothing*/}
  
};
