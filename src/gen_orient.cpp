/*function [orient count outcluster removed Ordered map]=gen_orient(tally, colweight, makernd, filton, weighting)
% Call form: function [orient count outcluster removed Ordered altchov map]=gen_orient(tally, colweight, makernd, filton, weighting)
% This function implements a hierarchical orientation method that
% orients in-place and re-evaluates at each step edge weights on the graph.
% It has been formated to use a sparse matrix-style formulation for
% creating weights, adding a good and bad matrix together.
%
% We are using a +/-1 orientation NOT 1/0
%
% Input:
%   tally - a nx6 tally file-- remaps to minimized values
%          Can only compute on nx6, partially designed for nx4 (fixes it)
%   colweight - weighting for columns 5,6 of tally file
%   makernd  -- 3 Options for alternative merge choices are decided:
%               0-- None, takes first "max" found.
%               1-- Random, chooses randomly among all "equal" weights
%               2-- Findbest, uses more data to try and find a best choice
%               3-- Performs a "simultaneous" merge of all equal weights
%               --Default is 2, best from experimental results--
%   filton   -- Option for if simple filtering is on.
%               The filter called in this version (2) does less, due to
%               expecting a specific weighting scheme. If using more simple
%               schemes, some extra filtering may be desired. See filter
%               file for more details.
%               --Default is ON ==1--
%   weighting-- Requires a reference to another matlab function that
%               performs weighting. Function must take 2 types of input:
%               weighting(Good, Bad)  & weighting(Good, Bad, x).
%
% Output:
%   orient -  the orientation found-- standarized to minimize changes
%   count  -  the bad count for mate-pairs
%   outcluster- Structure with disconnected components, size, members,
%                   and orientation.
%   removed - list of edges removed due to basic filtering rules.
%   Ordered - ordered list of joins to make a dendrogram
%   map     - the end map used for going from contig -> indexes
%               (after filtering/data cleaning)
%
% Dependencies:
%   maptally3 -- remaps tally file to reduced labels.
%   findbadcount -- this is implemented, but doesn't work with nx4 tally
%   (following dependencies are based on choosen options)
%   simplefilter2   -- this does a few different filtering and data
%                       cleaning operations.
%   findbestmerge2  -- this returns a single best merge to perform
%   merge_all_equal -- this performs all equal merges at once
%
% Created April 26, 2012 by Karl Schmitt
%
% Revision History:
%   Completely revised calculation methods to use all clusters!
%   5/11/12 -- Reworked acceptance of nx4 tallys. Modified Locally.
%   6/12 -- Included Randomization option for alternative joins
%   7/17/12 -- Modified output to minimize orientation changes per component
%   7/17/12 -- Added output of clusters, including members, size and orient
%   7/30/12 -- Move printout of components to wrapper func. Still output.
%   8/10/12 -- Added a simple filter and output of removed.
%   8/27/12 -- Added a toggle for the simple filter. Default is on (1)
%   1/13    -- Generalized function to take ANY weighting scheme.
%   2/13    -- Cleaned up comments and text for distribution.
%   3/4/13  -- Conversion to C++ code.

% To be Implement:
%   fixed   - any nodes we already know the orientation of, in a nx2 array
%                   THIS ISN'T IMPLEMENTED YET
*/

/*
%--------------------------------------------
% Declaration and Preparatory Calculations
%--------------------------------------------
*/

//%Remap tallyfile to reduced labels.

//Need a list of headers and stuff.
#include <iostream>
#include <string>
#include "gen_orient.h"
#include <stdio.h>
#include <fstream>
#include <limits>
#include <assert.h>

//Below are included in the header.
//#include <SparseMatrix.h>
//#include <Eigen/Dense>
//#include <Map.h>
//#include <list>
//#include <Eigen/Sparse>

typedef Eigen::Triplet<double> T;

//The three templated functions are overloaded to provide different returns of a maximum value from a sparse matrix
template<typename SpMat>
typename SpMat::Scalar sparse_max(const SpMat& m, typename SpMat::Index& maxRow, typename SpMat::Index& maxCol, int& count){
  typedef typename SpMat::Scalar scalar;
  typedef typename SpMat::Index index;
  typedef typename SpMat::InnerIterator iterator;

  scalar res= std::numeric_limits<scalar>::min();
  scalar oldres= res;

  count=0;

  for(index k=0; k<m.outerSize(); ++k)
    for(iterator it(m,k);it;++it)
      {
	res=std::max(res,it.value());
	if(res==it.value() && res!=oldres)
	  {
	    count=0;
	    maxRow=it.row();
	    maxCol=it.col();
	  }
	if(res==it.value())
	  {count++;}
	oldres=res;
      }
  return res;
}

template<typename SpMat>
typename SpMat::Scalar sparse_max(const SpMat& m){
  typename SpMat::Index maxRow;
  typename SpMat::Index maxCol;
  int count;

  return sparse_max(m, maxRow, maxCol, count);
}

template<typename SpMat>
typename SpMat::Scalar sparse_max(const SpMat& m, typename SpMat::Index& maxRow, typename SpMat::Index& maxCol){
  int count;

  return sparse_max(m, maxRow, maxCol, count);
}



using namespace Eigen;
using namespace std;

//A few global variables
int N; // The number of contigs
int EdgeCnt; // The number of edges in tally file
string outlbl="outort"; //default output label
string fntally="orientations_tallies"; //Default input file.

int main(int argc, const char **argv)
{
  double alphaVar=0; //Weighting for columns 5/6 of tally file. Default 0
  string choicemethod ("least"); //How we decide between "equal" choices. See specifications.
  int filters=1;     //Should basic data clean-up go on. Default-on.
  int nxbad,nxgood,nybad,nygood; //these will hold values for row/column sums for comparision
  string temps;
  double tmpdbl;


  //Read in Arguments
  for (int acnt=1;acnt<argc;acnt++)
    {
      temps=argv[acnt];
      if (temps=="-w")
	{sscanf(argv[acnt+1],"%lf",&alphaVar);}
      else if(temps=="-o")
	{ outlbl=argv[acnt+1];}
      else if(temps=="-in")
	{ fntally=argv[acnt+1];}
      else if(temps=="-m")
	{ choicemethod=argv[acnt+1];}
      else if(temps=="-foff")
	{ filters=0;}


    }

  //Use old functions to read in data.
  getdatasize();

  MatrixXd tally(EdgeCnt,6); //Matrix holding tally  file
  VectorXd map(N); //array holding contig numbers to demap genes
  VectorXd temp(N); // a second array for holding map after filtering
  VectorXd map2;
  MatrixXd removed; // matrix to hold the removed elements from filtering
  int reduceN;     //number of unique contigs after filtering and remapping

  readdata(tally,map);

   if (filters==1)
    {
      //Perform some basic filtering.
      simplefilter2(tally);

      reduceN=maptally(tally,temp);

      //This should initialize then fill our new map2, getting rid of empty values.
      map2.resize(reduceN);
      for(int ii=0;ii<reduceN;ii++)
	{ map2(ii)=temp(ii);}

      //      new (&map2) Matrix<double, Dynamic, Dynamic>(temp(0,reduceN-1));
    }
  else
    { map2=map; reduceN=N;}

  VectorXd myort;
  myort.setOnes(reduceN);  //%orientation, pre-declared
  list<int> cluster[reduceN];  // an array of linked-lists to hold members of a cluster
  list<int>::iterator clustIT;  //An iterator we'll use on the clustered list.

   //insert initial element
  for(int ii=0;ii<reduceN;ii++)
    { cluster[ii].push_back(ii);}

  //Make sparse matrices to operate on.
  vector<T> tripletList_good;
  vector<T> tripletList_bad;
  vector<T> tripletList_Dist;
  double tmpdst; //holds a temporary distance for storage
  double tmpgd,tmpbd; //holds temporary good/bad for readability.

  //Pre-allocate space for the triplet-lists
  tripletList_good.reserve(2*tally.rows());
  tripletList_bad.reserve(2*tally.rows());
  tripletList_Dist.reserve(2*tally.rows());

  //This loop puts all the data into the triplets from the tally-file
  for(int ii=0;ii<tally.rows();ii++)
    {
      tmpgd= tally(ii,2)+alphaVar*tally(ii,4);
      tmpbd= tally(ii,3)+alphaVar*tally(ii,5);

      //Setup things to make good sparse matrix
      if(tally(ii,2)>0 || tally(ii,4)>0)
	{
	  tripletList_good.push_back(T(tally(ii,0),tally(ii,1),tmpgd));
	  tripletList_good.push_back(T(tally(ii,1),tally(ii,0),tmpgd));
	}

      //Setup things to make bad sparse matrix
      if(tally(ii,3)>0 || tally(ii,5)>0)
	{
	  tripletList_bad.push_back(T(tally(ii,0),tally(ii,1),tmpbd));
	  tripletList_bad.push_back(T(tally(ii,1),tally(ii,0),tmpbd));
	}

      //Setup things to make Distt sparse matrix
      tmpdst= (tmpgd-tmpbd)*(tmpgd-tmpbd)/(tmpgd+tmpbd);
      tripletList_Dist.push_back(T(tally(ii,0),tally(ii,1),tmpdst));
      tripletList_Dist.push_back(T(tally(ii,1),tally(ii,0),tmpdst));
    }

  //Declare each of the sparse matrices we want to use...
  SpMat Good(reduceN,reduceN);
  Good.setFromTriplets(tripletList_good.begin(),tripletList_good.end());
  SpMat Bad(reduceN,reduceN);
  Bad.setFromTriplets(tripletList_bad.begin(),tripletList_bad.end());
  SpMat Distt(reduceN,reduceN);
  Distt.setFromTriplets(tripletList_Dist.begin(),tripletList_Dist.end());

  //A temporary sparse vector and matrix for clarity of operations later
  SparseVector<double,RowMajor> tmpvec(reduceN);
  SpMat tmpmat(reduceN,reduceN);

  //Clean-up any extra space in the sparse matrices.
  Good.makeCompressed();
  Bad.makeCompressed();
  Distt.makeCompressed();

  //Calculate the distance matrix
  Distt= ( ( Good-Bad).cwiseProduct(Good-Bad) ).cwiseQuotient(Good+Bad );

  //Find the "max" of the matrix
  //  double Dmax; //will hold our maximum value.
  int count=0; //Use this to track counts of maximum
  SpMat::Index maxRow,maxCol,x,y;  //Holders for the location of the maximum

  //Have to use our own functions (see templates above) to find this.
  sparse_max(Distt, maxRow, maxCol, count);

  //NOT IMPLEMENTED-- JOIN TRACKING.
  //Ordered=zeros(ncnt,2);
  //ind=1;

  // Some debugging lines
  //  cout<<"DISTT\n";
  //cout<<Distt<<endl;
  //cout<<"GOOD\n";
  //cout<<Good<<endl;

  //For tracking alternate joins. Only if they happen, removing tracking what they were.
  int altjncnt=0;
  int altcho=0;

  //-------------------
  // Algorithm Body
  //-------------------

  while(Distt.nonZeros()>0)
    {
      //cout<<"Good"<<endl<<Good<<endl;
      //cout<<"Bad"<<endl<<Bad<<endl;
      //cout<<"Distt"<<endl<<Distt<<endl;

      //Do some stuff with the multiple joins possible.
      if(count>2)
        {
	  altjncnt++;

	  //Based on default or input parameter, choose a maximum value to join on.
	  if(choicemethod=="least")
	  //Use the least happiness lost method
	    { altcho=findbestmerge2(maxRow,maxCol, Distt, Good, Bad, cluster); }
          else if(choicemethod=="all")
	    //Perform ALL equal-weight merges at once.
	    //Function modifies ALL parameters passed.
	    //
	    //  NOT IMPLEMENTED!
	    {  }

	  //This is the old Matlab code which can be converted into C++ later.
	    /*
	    merge_all_equal([multijnsx multijnsy],Good, Bad, Distt, cluster, myort);

            altchov(end+1)=acho;

            Ordered(ind:ind+size(madejns,1)-1,1:2)=madejns;

            ind=ind+size(madejns,1);

            %Calculate next joining to do. We do this here for loop control ending.
            [weight(:,1) weight(:,2)]=max(Distt,[],2);

            continue;
	    /
	    { actually do nothing/}
	  else if(choicemethod.compare("first"))
	    //Use the first one returned. This is what we already found. I.e. do nothing.
	    { nothing/}
	    */
        }


      x=maxRow;
      y=maxCol;

      /* commenting out tracking of joins
    	 Ordered(ind,1)=x;
	 Ordered(ind,2)=y;
	 ind=ind+1;
      */

      //Because we've been collapsing the good/bads, these are the same for
      //unclustered OR clustered elements.

      //cout<<"Good"<<endl<<Good<<endl;
      //cout<<"Bad"<<endl<<Bad<<endl;

      bool noflip=false;
      if( Good.coeff(x,y)>=Bad.coeff(x,y))
	{  noflip=true;}

      //sum over edges to get good/bad counts

      nxbad=Bad.row(x).sum();
      nxgood=Good.row(x).sum();
      nybad=Bad.row(y).sum();
      nygood=Good.row(y).sum();

      //cout<<"GOOD\n"<<Good<<endl;
      //    %Zero out self-reference entries.
      Good.coeffRef(x,y)=0;
      Good.coeffRef(y,x)=0;
      Bad.coeffRef(x,y)=0;
      Bad.coeffRef(y,x)=0;

      //cout<<"Good"<<endl<<Good<<endl;
      //cout<<"Bad"<<endl<<Bad<<endl;

      //cout<<"GOOD\n"<<Good<<endl;
      //    %Temp holders so we can do inplace storage ... I don't think we need these.
      //tgoodx=Good(x,:);
      // tgoody=Good(y,:);
      //tbadx=Bad(x,:);
      //tbady=Bad(y,:);

      if(noflip)
	{
	  //        %This means the orientation as is is FINE. So we do not change
	  //        %orientations in that array, but DO update the weights in good/bad

	  //        %Update Good/Bad/Weight
	  tmpvec=Good.row(x)*cluster[x].size()+Good.row(y)*cluster[y].size();
	  tmpvec=tmpvec/(cluster[x].size()+cluster[y].size());
	  for(SparseVector<double,RowMajor>::InnerIterator it(tmpvec);it;++it)
	    { Good.coeffRef(x,it.index())=it.value(); }

	  tmpvec=Bad.row(x)*cluster[x].size()+Bad.row(y)*cluster[y].size();
	  tmpvec=tmpvec/(cluster[x].size()+cluster[y].size());
	  for(SparseVector<double,RowMajor>::InnerIterator it(tmpvec);it;++it)
	    { Bad.coeffRef(x,it.index())=it.value(); }
	}
      else
	{
	  //%Decide which has less impact (or a better impact). If y-bad is
	  //%significantly larger than y-good it'll be NEGATIVE, meaning it
	  //%should get flipped...
	  if(  (nxgood==0 && nygood==0 && nybad>nxbad) || (nxgood-nxbad>nygood-nybad && nxgood!=0))
	    {
	      //%Flip Y
	      //%myort(x) -- doesn't change, so leave it alone.
	      //%To change y's orts, we change all members. multiply by -1 to
	      //%flip it.
	      for(clustIT=cluster[y].begin();clustIT!=cluster[y].end();clustIT++)
		{
		  myort(*clustIT)*=-1;
		}


	      tmpvec= (Good.row(x)*cluster[x].size()+Good.row(y)*(cluster[y].size()))/(cluster[x].size()+cluster[y].size());
	      for(SparseVector<double,RowMajor>::InnerIterator it(tmpvec);it;++it)
		{ Good.coeffRef(x,it.index())=it.value(); }

	      //cout<<"GOOD\n"<<Good<<endl;
	
	      tmpvec=Bad.row(x)*cluster[x].size()+Bad.row(y)*cluster[y].size();
	      tmpvec=tmpvec/(cluster[x].size()+cluster[y].size());
	      for(SparseVector<double,RowMajor>::InnerIterator it(tmpvec);it;++it)
		{ Bad.coeffRef(x,it.index())=it.value(); }

	    }
	  else
	    {
	      //%Flip X
	      for(clustIT=cluster[x].begin();clustIT!=cluster[x].end();clustIT++)
		{ myort(*clustIT)*=-1;	}

	      //%Update Good/Bad/Weight
	      //cout<<"GOOD\n"<<Good<<endl;
	      tmpvec=Good.row(x)*cluster[x].size()+Good.row(y)*cluster[y].size();
	      tmpvec=tmpvec/(cluster[x].size()+cluster[y].size());
	      for(SparseVector<double,RowMajor>::InnerIterator it(tmpvec);it;++it)
		{ Good.coeffRef(x,it.index())=it.value(); }

	      tmpvec=Bad.row(x)*cluster[x].size()+Bad.row(y)*cluster[y].size();
	      tmpvec=tmpvec/(cluster[x].size()+cluster[y].size());
	      for(SparseVector<double,RowMajor>::InnerIterator it(tmpvec);it;++it)
		{ Bad.coeffRef(x,it.index())=it.value(); }

	    }


	}

      // cout<<"Good"<<endl<<Good<<endl;
      //cout<<"Bad"<<endl<<Bad<<endl;
      //cout<<"Distt"<<endl<<Distt<<endl;


      //for(int kk=0;kk<Good.rows();++kk)
      //Store column-wise values. Have to do so explicitly.
      //cout<<Good<<endl;
      //cout<<Bad<<endl;
      for(int pp=0;pp<Good.cols();++pp)
	{
	  tmpdbl=Good.coeff(x,pp);
	  Good.coeffRef(pp,x)=tmpdbl;
	  tmpdbl=Bad.coeff(x,pp);
	  Bad.coeffRef(pp,x)=tmpdbl;
	  Good.coeffRef(pp,y)=0;
	  Bad.coeffRef(pp,y)=0;
	}

      //cout<<"Good"<<endl<<Good<<endl;
      //cout<<"Bad"<<endl<<Bad<<endl;


      //cout<<"Good"<<endl<<Good<<endl;


      //%Zero out good/bad old values and update distance matrix
      Good.row(y)*=0;
      //Good.col(y)*=0;
      Bad.row(y)*=0;
      //Bad.col(y)*=0;

      //Clean up any zero's we put in.
      Good.prune(0,0.00000000001);
      Bad.prune(0,0.00000000001);

      //%Have to use our updated Good/Bad NOT the temp!!
      Good.makeCompressed();
      Bad.makeCompressed();

      tmpvec=(Good.row(x)-Bad.row(x)).cwiseProduct(Good.row(x)-Bad.row(x));
      //tmpvec=tmpvec.cwiseQuotient(Good.row(x)+Bad.row(x));
      //cout<<"dist temp"<<endl<<tmpvec<<endl;
      for(SparseVector<double,RowMajor>::InnerIterator it(tmpvec);it;++it)
	{ Distt.coeffRef(x,it.index())=it.value(); }

      Distt.row(y)*=0;

      //cout<<"Distt"<<endl<<Distt<<endl;
      //Distt.col(y)*=0;

      //Store column-wise values. Have to do so explicitly.
      for(int pp=0;pp<Distt.cols();++pp)
	{
	  Distt.coeffRef(pp,x)=Distt.coeff(x,pp);
	  Distt.coeffRef(pp,y)=0;
	}

      Distt.prune(0,0.00000000001);
      Distt.makeCompressed();

      //%add nodes to a new cluster
      //cluster(x).size=cluster(x).size+cluster(y).size;
      cluster[x].merge(cluster[y]); //members=[cluster(x).members cluster(y).members];

      //By the STL, cluster[y] should now be empty, so we don't need the next piece of code. Left for reference for now.

      //%for carefulness-- zero out cluster(y)
      //cluster(y).size=0;
      //cluster(y).members=[];

      //%Calculate next joining to do. We do this here for loop control ending.
      //[weight(:,1) weight(:,2)]=max(Distt,[],2);
      sparse_max(Distt, maxRow, maxCol, count);
      //maxRow=maxLoc.row();
      //maxCol=maxLoc.col();
    }

  //%------------------------
  //% Formatting and Output
  //%------------------------

  //%Get the number of connected components. Combined components are zeroed
  //%out earlier.
  VectorXd clusterzero;
  clusterzero.setOnes(reduceN);
  clusterzero*=0;

  for(int jj=0;jj<reduceN;jj++)
    {
      if (cluster[jj].size()>0)
	{ clusterzero(jj,0)=1;} //comps=find([cluster(:).size]>0);
    }

  //%Output some report information. Alternative solutions possible and # of
  //%connected components.
  if(clusterzero.sum()>1)
    {    cout<< clusterzero.sum() << " of components\n";}
  //    %Print some stuff in different files or soemthing.


  if( altjncnt>0)
    {
      cout<<altjncnt<< " alternate joins instances occured\n";
      cout<<altcho<<" alternate choices in alt joins\n";
    }


  //%Standard Output and save files for each component.

  //%Final output
  //outcluster=cluster(comps);

  //%Relabel the out-cluster in terms of originally input tally file.
  //not relabeling. still need loop to normalize orientations for minimum flips.

  for(int  ii=0; ii<reduceN;ii++)
    {
      if(cluster[ii].size()>0)
	{

	  //%standardize component outputs. Since each component is disconnected,
	  // %You can flip orientations IN a component without changing relative
	  // %orientations. Orientations are -1/1 so positive sum means More
	  // %changes.

	  //sum the orientations
	  int tmp=0;
	  for(clustIT=cluster[ii].begin();clustIT!=cluster[ii].end();clustIT++)
	    {tmp+=*clustIT;}
	  if (tmp<0)
	    {
	      for(clustIT=cluster[ii].begin();clustIT!=cluster[ii].end();clustIT++)
		{myort(*clustIT)*=-1;}
	    }
	}
    }


  ofstream out;
  out.open(outlbl.c_str());
  for(int ii=0; ii<reduceN;ii++)
    {out<< map(ii) << " "<< myort(ii)<<endl; }

  out.close();

  /*
tmp+=*clustIT;}

	  outcluster(ii).members=sort(cluster(comps(ii)).members);
    if sum(myort(outcluster(ii).members) ) > 0
        myort(outcluster(ii).members)=myort(outcluster(ii).members)*-1;
    end

    outcluster(ii).orient=myort(outcluster(ii).members);

    outcluster(ii).members=map(outcluster(ii).members);
end
     */

  //orient=[map, myort];
  //count=findbadcount(newtallies,colweight,orient(:,2));
	
}
	
/*
========================
*/
void getdatasize()
{
  FILE *tFile;
  //int **tallies;
  double temp1,temp2, d1,d2;
  int w1,w2, unq1,unq2;
  double *exist;

  //Open all the files we are going to read
  tFile=fopen(fntally.c_str(),"r");

  if(tFile==NULL)
    {
      cout<<"Tally file error!";
      //	system("pause");
      exit(EXIT_FAILURE);
    }


  //Scan tally file to find out how many edges there are
  EdgeCnt=0;
  while(fscanf(tFile,"%lf %lf %d %d %lf %lf",&temp1, &temp2, &w1, &w2, &d1, &d2)!=EOF)
    {EdgeCnt++;}

  rewind(tFile);

  exist=(double *)malloc(2*EdgeCnt*sizeof(double));

  //Get a count of how many unique elements (N) are in the contigData file and keep track of what is there
  N=0;
  while(fscanf(tFile,"%lf %lf %d %d %lf %lf",&temp1, &temp2, &w1, &w2, &d1, &d2)!=EOF)
    {
      unq1=1;
      unq2=1;
      for(int i=0;i<N;i++)
	{
	  if(exist[i]==temp1)
	    {unq1=0;}
	  if(exist[i]==temp2)
	    {unq2=0;}
	}
      if(unq1)
	{
	  exist[N]=temp1;
	  N++;
	}
      if(unq2)
	{
	  exist[N]=temp2;
	  N++;
	}
    }

  free(exist);
  fclose(tFile);


  //oEdgeCnt=EdgeCnt;
  //oN=N;
}

// Precondition: getdata() has been called. N has been set.
// Postcondition: tallies will hold all the contig-contig & weight data, in reduced contig numbers (1-#contig)
// Postcondition: map will hold an index file with map(x) == original contig label for the contig labeled x in tallies

void readdata(Eigen::MatrixXd& tallies, Eigen::VectorXd& map)
{
  FILE *tFile;
  double temp1,temp2;
  int ind1, ind2;
  int w1,w2, w3, w4, unq1,unq2;
  int locN=0;



  //Open all the files we are going to read
  tFile=fopen(fntally.c_str(),"r");

  if(tFile==NULL)
    {
      cout<<"Tally file error!";
      //	system("pause");
      exit(EXIT_FAILURE);
    }


  //Get a count of how many unique elements (N) are in the contigData file and keep track of what is there

  while(fscanf(tFile,"%lf %lf %d %d %d %d",&temp1, &temp2, &w1, &w2, &w3, &w4)!=EOF)
    {
      unq1=1;
      unq2=1;
      for(int i=0;i<locN;i++)
	{
	  if(map(i)==temp1)
	    {unq1=0;}
	  if(map(i)==temp2)
	    {unq2=0;}
	}
      if(unq1)
	{
	  map(locN)=temp1;
	  locN++;
	}
      if(unq2)
	{
	  map(locN)=temp2;
	  locN++;
	}
    }
  rewind(tFile);

  ind1=-1;
  ind2=-1;

  //Rescan the tally/data file and save it. This time we remap all the contigs to new, smaller index values
  // for where in the orientation arrays the orientation is

  int edgind=0;
  while(fscanf(tFile,"%lf %lf %d %d %d %d",&temp1, &temp2, &w1, &w2, &w3, &w4)!=EOF)
    {
      if(temp1==temp2)
	{EdgeCnt--;continue;}
      for(int i=0;i<locN;i++)
	{
	  if(map(i)==temp1)
	    {
	      ind1=i;
	      break;
	    }
	}
      for(int i=0;i<locN;i++)
	{
	  if(map(i)==temp2)
	    {
	      ind2=i;
	      break;
	    }
	}

      if(ind1==-1 || ind2==-1)
	{cout<<"Error making map of tally file"<<endl;system("pause");exit(EXIT_FAILURE);}


      tallies(edgind,0)=ind1;
      tallies(edgind,1)=ind2;
      tallies(edgind,2)=w1;
      tallies(edgind,3)=w2;
      tallies(edgind,4)=w3;
      tallies(edgind,5)=w4;
      edgind++;

      ind1=-1;
      ind2=-1;
    }

  fclose(tFile);

  //cout<<tallies<<endl;
  /*
  //Test read
  for(int ii=0;ii<tallies.rows();ii++)
    {
      for(int jj=0;jj<6;jj++)
	{fprintf(stdout,"%d ",tallies(ii,jj));}
      fprintf(stdout,"\n");
    }
  */

  return;
}


int maptally(Eigen::MatrixXd& tally, Eigen::VectorXd& map)
{
  /* This function relabels a tally to reduce the indexing so we can index directly to the element we are using */

  //MatrixXd temptally=tally; //For temporary copying/moving of the tally
  VectorXd used(2*tally.rows()); //To track which values ahve been used.
  //int replace; // temporary holder for the value we are updating.
  int temp1,temp2,unq1,unq2,ind1,ind2;
  int locN=0;

  map*=0;

  //This identifies the count of unique elements in tally
  for(int ii=0;ii<tally.rows();ii++)
    {
      temp1=tally(ii,0);
      temp2=tally(ii,1);

      unq1=1;
      unq2=1;
      for(int jj=0;jj<locN;jj++)
	{
	  if(map(jj)==temp1)   {unq1=0;}
	  if(map(jj)==temp2)   {unq2=0;}
	}
      if(unq1)	{ map(locN)=temp1; locN++; }
      if(unq2)	{ map(locN)=temp2; locN++; }
    }

  ind1=-1;  ind2=-1;

  //Reparse through the tally file, relabeling things now.

  int edgind=0;
  for(int ii=0;ii<tally.rows();ii++)
    {
      temp1=tally(ii,0);
      temp2=tally(ii,1);

      for(int ij=0;ij<locN;ij++)
	{
	  if(map(ij)==temp1)
	    { ind1=ij; break; }
	}
      for(int ij=0;ij<locN;ij++)
	{
	  if(map(ij)==temp2)
	    { ind2=ij; break; }
	}

      if(ind1==-1 || ind2==-1)
	{cout<<"Error making map of tally file"<<endl;exit(EXIT_FAILURE);}

      tally(edgind,0)=ind1;
      tally(edgind,1)=ind2;
      edgind++;

      ind1=-1;   ind2=-1;
    }

  return locN;
}


/* This function performs a simple filtering on the data. Merging any duplicate edges, removing edges that are
0/1 or 1/0, and removing any 1's for edges that are higher weight. If input data has things in column 5/6, this might
generate some un-predicted or desired behavior.
*/
void simplefilter2(Eigen::MatrixXd& tally)
{
  VectorXd rmv(tally.rows());
  rmv.setZero();

  //This section finds any duplicate edges
  for(int jj=0;jj<EdgeCnt;jj++)
    {
      for(int ii=jj+1;ii<EdgeCnt;ii++)
	{
	  if ( (tally(jj,0)==tally(ii,0) && tally(jj,1)==tally(ii,1)) || (tally(jj,0)==tally(ii,1) && tally(jj,1)==tally(ii,0)) )
	    {
	      rmv(ii)=1;
	      tally(jj,2)+=tally(ii,2);
	      tally(jj,3)+=tally(ii,3);
	      tally(jj,4)+=tally(ii,4);
	      tally(jj,5)+=tally(ii,5);
	      tally.row(ii).setZero();
	    }
	}
      //Note this might have an error depending on complexity of input data. NOT considering columns 5/6
      if( (tally(jj,2)==1 && tally(jj,3)==0 ) || (tally(jj,3)==1 && tally(jj,2)==0)||  tally(jj,2)==tally(jj,3) ||tally(jj,0)==tally(jj,1) )
	{
	  cout<<tally.row(jj)<<endl;
	  rmv(jj)=1;
	  tally.row(jj).setZero();
	}


      //This is our "zeroing out" section. If edge has */1 or 1/* with *>=1 we zero out the '1'. Weaker?Stronger?
      if( tally(jj,2)==1 && tally(jj,3)>1 )
	{ tally(jj,2)=0;}
      if( tally(jj,3)==1 && tally(jj,2)>1 )
	{ tally(jj,3)=0;}



    }


  //This section copies row by row the old tally into a new, temporary tally. Then replaces the old tally.
  if(rmv.any())
    {
      MatrixXd temp(tally.rows()-rmv.count(),6);

      int nxtidx=0;
      for(int ii=0; ii<tally.rows();ii++)
	{
	  if(!rmv(ii))
	    {
	      temp.row(nxtidx)=tally.row(ii);
	      nxtidx++;
	    }
	}

      tally=temp;
    }

}

//int findbestmerge2(int& maxRow, int& maxCol, SpMat& Distt, SpMat& Good, SpMat& Bad, list<int>*  cluster)
int findbestmerge2(SpMat::Index& maxRow, SpMat::Index& maxCol, SpMat& Distt, SpMat& Good, SpMat& Bad, std::list<int>*  cluster)
{
  //This function will return (in maxRow/maxCol) the location of the link to merge over.

  //SpMat::InnerIterator maxLoc;
  double Dmax;
  int count=0;
  Dmax=sparse_max(Distt, maxRow, maxCol, count);
  /*  int count=0;
  for(int jj=0;jj<Distt.outerSize();++jj)
    {
      for(SpMat::InnerIterator it(Distt,jj);it;++it)
	{ if(it.value()==Dmax){count++;} }
    }
  */
  int tocheckcnt=count;
  VectorXd maxfuture(tocheckcnt); //Holds the maximum happiness possible for that join
  maxfuture.setZero();
  VectorXd xindx(tocheckcnt),yindx(tocheckcnt); //Will hold our indexes we are checking.
  double nxbad,nxgood,nybad,nygood, Ghold,Bhold;
  bool noflip;

  VectorXd tmp1(Good.rows());
  VectorXd tmp2(Good.rows());
  assert(Good.rows() == Distt.rows());
  assert(Good.cols() == Distt.cols());
  cout << "tmp1.size" << tmp1.rows() << " " << tmp1.size() << "\n";

  //  cout<<Distt<<endl<<Distt.maxCoeff()<<endl;
  //cout<<Distt<<endl<<Distt.maxCoeff(&maxRow,&maxCol)<<endl;

  int i;
  VectorXd::Index indx=0;
  for(i=0;i<Distt.outerSize();++i)
    {//Iterate over all of Distt matrix
      for(SpMat::InnerIterator it(Distt,i);it;++it)
	{

	  //  cout<<"Good"<<endl<<Good<<endl;
	  //cout<<"Bad"<<endl<<Bad<<endl;

	  if(it.value()==Dmax)
	    {
	      noflip=false;
	      if(Good.coeff(it.row(),it.col())>=Bad.coeff(it.row(),it.col() )  )
		{ noflip=true;}
	
	      //Get sums of rows for deciding which to flip.
	      //Putting these in if statements to reduce execution.
	      /*
		nxbad=Bad.row(i).sum();
		nybad=Bad.row(j).sum();
		nxgood=Good.row(i).sum();
		nygood=Good.row(j).sum();
	      */
	
	      //Temporarily hold values we are zero'ing
	      Ghold=Good.coeff(it.row(),it.col());
	      Bhold=Bad.coeff(it.row(),it.col());
	
	      //Zero out the merged edge
	      Good.coeffRef(it.row(),it.col())=0;
	      Good.coeffRef(it.col(),it.row())=0;
	      Bad.coeffRef(it.row(),it.col())=0;
	      Bad.coeffRef(it.col(),it.row())=0;

	      //	      cout<<"Bad"<<endl<<Bad<<endl;

	      if(noflip)
		{//This means we aren't flipping any orientations. Do a fake merge, without division

                  auto sgrow = Good.row(it.row())*cluster[it.row()].size();
                  cout << "Good row:" << Good.row(it.row())
                       << "Cluster size:" << cluster[it.row()].size() << "\n"
                       << "Result:" << sgrow;
                  auto sgrow2 = Good.row(it.col())*cluster[it.col()].size();
                  cout << "Good row2:" << Good.row(it.col())
                       << "Cluster size:" << cluster[it.col()].size() << "\n"
                       << "Result:" << sgrow2;
                  auto sum = sgrow + sgrow2;
                  cout << "Sum:" << sum;
                  //		  tmp1=Good.row(it.row())*cluster[it.row()].size()+Good.row(it.col())*cluster[it.col()].size();
                  VectorXd tmp(sum);
                  cout << "tmp:" << tmp << " " << tmp.size() << "\n";
		  cout<<"Bad"<<endl<<Bad<<endl;

		  // cout<<tmp1<<endl<<tmp2<<endl<<Bad.row(it.row())<<endl<<Bad.row(it.col())<<endl;
		  tmp2=Bad.row(it.row())*cluster[it.row()].size()+Bad.row(it.col())*cluster[it.col()].size();
		  //cout<<tmp1<<endl<<tmp2<<endl<<Bad.row(it.row())<<endl<<Bad.row(it.col())<<endl;

		  //cout<<"Bad"<<endl<<Bad<<endl;

		}
	      else
		{
		  nxbad=Bad.row(it.row()).sum();
		  nybad=Bad.row(it.col()).sum();
		  nxgood=Good.row(it.row()).sum();
		  nygood=Good.row(it.col()).sum();
		  //Decide the best one to merge
		  if( (nxgood==0 && nygood==0 && nybad>nxbad) || ( (nxgood-nxbad)>(nygood-nybad) && nxgood!=0) )
		    { //Means we should have flipped Y. Sum accordingly
		      tmp1=Good.row(it.row())*cluster[it.row()].size()+Bad.row(it.col())*cluster[it.col()].size();
		      tmp2=Bad.row(it.row())*cluster[it.row()].size()+Good.row(it.col())*cluster[it.col()].size();
		    }
		  else
		    { //We should have flipped X. Sum according
		      tmp1=Bad.row(it.row())*cluster[it.row()].size()+Good.row(it.col())*cluster[it.col()].size();
		      tmp2=Good.row(it.row())*cluster[it.row()].size()+Bad.row(it.col())*cluster[it.col()].size();
		    }
		}

	      cout<<"Bad"<<endl<<Bad<<endl;
	      //Store our check values and findings.
	      xindx(indx)=it.row();
	      yindx(indx)=it.col();
	      maxfuture(indx)= (tmp1-tmp2).array().abs().sum();
	      indx++;

	      //Restore Values
	      Good.coeffRef(it.row(),it.col())=Ghold;
	      Good.coeffRef(it.col(),it.row())=Ghold;
	      Bad.coeffRef(it.row(),it.col())=Bhold;
	      Bad.coeffRef(it.col(),it.row())=Bhold;

	    }
	
	}

    }

  int maxmerge;
  maxmerge=maxfuture.maxCoeff(&indx);
  maxRow=xindx(indx);
  maxCol=yindx(indx);

  if( (maxfuture.array()==maxmerge).count()>2)
    { return 1;}
  else
    { return 0;}

	
}

/*

//Initializing max returned to zero, because, in OUR case, that should be the minimum possible value.
// This function can NOT be re-used for any matrix max.
double mymax(SpMat& inmatx)
{
  double cmax=0;

  for(int ii=0;ii<inmatx.outerSize();++ii)
    {
      for(SpMat::InnerIterator it(inmatx,ii);it;++it)
	{
	  if(it.value()>cmax)
	    {cmax=it.value();}
	}
    }
}

//This track how many we have.
double mymax(SpMat& inmatx,int& count)
{
  double cmax=0; //In our distance, always minimum is zero
  count=0;//set the count to zero.

  for(int ii=0;ii<inmatx.outerSize();++ii)
    {
      for(SpMat::InnerIterator it(inmatx,ii);it;++it)
	{
	  //if we found a bigger value, update max & reset count
	  if(it.value()>cmax)
	    {cmax=it.value();count=0;}
	  //if we found an equal value increment count
	  if(it.value()==cmax)
	    {count++;}
	}
    }
}

//this tracks the count and returns the location of one max
double mymax(SpMat& inmatx,int& count, SpMat::Index& maxRow, SpMat::Index& maxCol)
{
  double cmax=0; //In our distance, always minimum is zero
  count=0;//set the count to zero.

  for(int ii=0;ii<inmatx.outerSize();++ii)
    {
      for(SpMat::InnerIterator it(inmatx,ii);it;++it)
	{
	  //if we found a bigger value, update max & reset count
	  if(it.value()>cmax)
	    {
	      cmax=it.value();
	      count=0;
	      maxRow=it.row();
	      maxCol=it.col();
	    }
	  //if we found an equal value increment count
	  if(it.value()==cmax)
	    {count++;}
	}
    }
}

//this tracks the count and returns the location of all maxes!
// This is far less efficient if we only care about ONE maximum.
//This function might not be efficient or necessary. Using a workaround now.

double mymax(SpMat& inmatx,int& count, MatrixXd::Index& maxRow, MatrixXd::Index& maxCol, int overspecify)
{
  double cmax=0; //In our distance, always minimum is zero
  count=0;//set the count to zero.

  for(int ii=0;ii<inmatx.outerSize();++k)
    {
      for(SpMat::InnerIterator it(inmatx,k);it;++it)
	{
	  //if we found a bigger value, update max & reset count
	  if(it.value()>cmax)
	    {
	      cmax=it.value();
	      count=0;
	      maxRow=it.row();
	      maxCol=it.col();
	    }
	  //if we found an equal value increment count
	  if(it.value()==cmax)
	    {count++;}
	}
    }
}
*/
