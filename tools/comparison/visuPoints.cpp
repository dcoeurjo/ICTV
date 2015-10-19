#include <iostream>
#include <cstdlib>
#include <boost/algorithm/string.hpp>
#include <map>
#include <vector>
#include <fstream>
#include <cmath>
#include <limits>

#include "DGtal/base/Common.h"
#include "DGtal/io/viewers/Viewer3D.h"
#include "DGtal/helpers/StdDefs.h"

#define NBDATA 3 /// x y z

typedef unsigned int uint;

struct convertGPUtoKhalimsky : std::unary_function <unsigned int, double>
{
  inline
  unsigned int operator() (const double& v) const
  { return std::floor(2.0*v); }
};

struct convertCPUtoKhalimsky : std::unary_function <unsigned int, double>
{
  inline
  unsigned int operator() (const double& v) const
  { return std::floor(v); }
};

template< typename Predicate >
bool loadFile(  const std::string& filename,
                DGtal::Viewer3D<>& viewer,
                const Predicate& predicate,
                const double radius )
{
  std::ifstream file( filename.c_str(), std::ifstream::in );
  std::string line;

  if( file.is_open())
  {
    std::cout << "Collecting data from file " << filename << " ..."<< std::endl;
    while( getline(file,line) )
    {
      /// Skip if the line contains a # char, or N, or any line too small (avoid the new line a end of file)
      if(line[0] == '#' || line[0] == 'N' || line.size() < NBDATA)
      {
        continue;
      }

      std::vector<std::string> words;
      boost::split(words, line, boost::is_any_of(" \t"), boost::token_compress_on);
      if(words.size() < NBDATA)
      {
        std::cout << "ERROR: I've got a wrong number of data on this line..." << std::endl;
        std::cout << "        ";
        for( uint i_word = 0; i_word < words.size(); ++i_word )
        {
            std::cout << words[i_word] << " ##### ";
        }
        std::cout << std::endl;
        std::cout << "Leaving now..." << std::endl;
        return false;
      }

      viewer.addBall( DGtal::Z3i::Point( predicate( std::atof(words[0].c_str())),
                                         predicate( std::atof(words[1].c_str())),
                                         predicate( std::atof(words[2].c_str())) ), radius );
    }
    std::cout << "... done." << std::endl;
    file.close();
  }
  else
  {
    std::cout << "ERROR: The file " << filename << " can't be opened. Is the file exists ?" << std::endl;
    std::cout << "Leaving now..." << std::endl;
    return false;
  }
  return true;
}

int main( int argc, char** argv )
{
  //// User's choice
  if ( argc == 1 )
  {
    std::cout << "Usage: " << argv[0] << "<fileGPU> <fileCPU> <radiusSphere>" << std::endl;
    std::cout << "       - computes the difference of results between the" << std::endl;
    std::cout << "         CPU version of the estimator and the GPU version" << std::endl;
    std::cout << "         GPU file : positions are between [0;size]." << std::endl;
    std::cout << "         CPU file : positions are between [0;2*size]" << std::endl;
    std::cout << "                                       or [-size;size]." << std::endl;
    std::cout << "         Error type : 1 is l_1, 2 is l_2, 3 is l_\\infty." << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << argv[ 0 ] << " file1.txt file2.txt 64 3" << std::endl;
    return 0;
  }

  std::string fileGPU = argc > 1 ? std::string( argv[ 1 ] ) : "file1.txt";
  std::string fileCPU = argc > 2 ? std::string( argv[ 2 ] ) : "file2.txt";
  double radiusSphere = argc > 3 ? std::atoi( argv[ 3 ] ) : 0.2;

  //// Variables initialization
  convertGPUtoKhalimsky predicateGPU;
  convertCPUtoKhalimsky predicateCPU;

  QApplication application(argc,argv);
  DGtal::Viewer3D<> viewer;
  viewer.show();

  //// Loading files
  viewer.setFillColor(DGtal::Color::Red);
  if( !loadFile( fileGPU, viewer, predicateGPU, radiusSphere ))
  {
    return 0;
  }

  viewer.setFillColor(DGtal::Color::Green);
  if( !loadFile( fileCPU, viewer, predicateCPU, radiusSphere ))
  {
    return 0;
  }

  viewer << DGtal::Viewer3D<>::updateDisplay;

  return application.exec();
}
