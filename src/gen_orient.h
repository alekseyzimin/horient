// Header File for Hierarchical Orientation Method

//Includes
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <list>

typedef Eigen::Triplet<double>T;
typedef Eigen::SparseMatrix<double,Eigen::RowMajor> SpMat;

//Function prototypes
void simplefilter2(Eigen::MatrixXd& tally);
int maptally(Eigen::MatrixXd& tally, Eigen::VectorXd& map);
int findbestmerge2(Eigen::ArrayXXd::Index& maxRow, Eigen::ArrayXXd::Index& maxCol, Eigen::ArrayXXd& Distt, Eigen::ArrayXXd& Good, Eigen::ArrayXXd& Bad, std::list<int>*  cluster);

//int findbestmerge2(int& maxRow, int& maxCol, SpMat& Distt, SpMat& Good, SpMat& Bad, std::list<int>*  cluster);

int findbestmerge2(SpMat::Index& maxRow, SpMat::Index& maxCol, SpMat& Distt, SpMat& Good, SpMat& Bad, std::list<int>*  cluster);

double mymax(SpMat& inmatx);
double mymax(SpMat& inmatx, int& count);
double mymax(SpMat& inmatx, int& count, SpMat::Index& maxRow, SpMat::Index& maxCol);

double mymax(Eigen::ArrayXXd& inmatx);
double mymax(Eigen::ArrayXXd& inmatx, int& count);
double mymax(Eigen::ArrayXXd& inmatx, int& count, Eigen::ArrayXXd::Index& maxRow, Eigen::ArrayXXd::Index& maxCol);

//double matmax(Eigen::ArrayXXd& inmatx, Eigen::ArrayXXd::Index& maxRow, Eigen::ArrayXXd::Index& maxCol);

//Data functions. First gets proper sizes to dyanmically allocate array sizes. Second
//   actually reads in contig data, and returns the edges with good/bad tallies, a mapping
//   of full-contig numbers to smaller, index values, and borient is the best orientation for checking
//   purposes.  Third reads in a pre-calculated join file for group crossovers/mutations
void getdatasize();
void readdata(Eigen::MatrixXd& tallies, Eigen::VectorXd& map);
