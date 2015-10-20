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

#include "helpers.h"

// template< typename Predicate >
// bool loadFile(  const std::string& filename,
//                 std::vector< Position* >& results,
//                 Position* barycenter,
//                 const Predicate& predicate )
// {
//   std::ifstream file( filename.c_str(), std::ifstream::in );
//   std::string line;
//
//   if( file.is_open())
//   {
//     std::cout << "Collecting data from file " << filename << " ..."<< std::endl;
//     while( getline(file,line) )
//     {
//       /// Skip if the line contains a # char, or N, or any line too small (avoid the new line a end of file)
//       if(line[0] == '#' || line[0] == 'N' )//|| line.size() < NBDATA)
//       {
//         continue;
//       }
//
//       std::vector<std::string> words;
//       boost::split(words, line, boost::is_any_of(" \t"), boost::token_compress_on);
//       if(words.size() < NBDATA)
//       {
//         std::cout << "ERROR: I've got a wrong number of data on this line..." << std::endl;
//         std::cout << "        ";
//         for( uint i_word = 0; i_word < words.size(); ++i_word )
//         {
//             std::cout << words[i_word] << " ##### ";
//         }
//         std::cout << std::endl;
//         std::cout << "Leaving now..." << std::endl;
//         return false;
//       }
//
//       Position *p = new Position();
//       p->x = predicate( std::atof(words[0].c_str()));
//       p->y = predicate( std::atof(words[1].c_str()));
//       p->z = predicate( std::atof(words[2].c_str()));
//
//       *barycenter += *p;
//
//       results.push_back( p );
//     }
//     std::cout << "Barycenter before: " << barycenter->x << "," << barycenter->y << "," << barycenter->z << "}" << std::endl;
//     *barycenter /= results.size();
//     std::cout << "Barycenter after: " << barycenter->x << "," << barycenter->y << "," << barycenter->z << "}" << std::endl;
//     std::cout << "Results.size() = " << results.size() << std::endl;
//     std::cout << "... done." << std::endl;
//     file.close();
//   }
//   else
//   {
//     std::cout << "ERROR: The file " << filename << " can't be opened. Is the file exists ?" << std::endl;
//     std::cout << "Leaving now..." << std::endl;
//     return false;
//   }
//   return true;
// }

template< typename Predicate >
bool showFile(  const std::vector< std::pair<Position*, Curvatures*> >& input,
                DGtal::Viewer3D<>& viewer,
                const Predicate& predicate,
                const double radius )
{
  for(  std::vector< std::pair<Position*, Curvatures*> >::const_iterator it=input.begin();
        it!=input.end(); ++it )
  {
    viewer.addBall( DGtal::Z3i::Point( predicate( *(it->first) ).x,
                                       predicate( *(it->first) ).y,
                                       predicate( *(it->first) ).z ), radius, 5 );
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
  double radiusSphere = argc > 3 ? std::atoi( argv[ 3 ] ) : 1.0;

  //// Variables initialization
  convertGPUtoKhalimsky predicateGPU;
  convertCPUtoKhalimsky predicateCPU;
  std::vector< std::pair<Position*, Curvatures*> > mapGPU;
  std::vector< std::pair<Position*, Curvatures*> > mapCPU;
  Position* barycenterGPU = new Position();
  Position* barycenterCPU = new Position();



  //// Loading files
  if( !loadFile( fileGPU, mapGPU, barycenterGPU, predicateGPU, 6 ))
  {
    deleteVector( mapGPU );
    return 0;
  }
  if( !loadFile( fileCPU, mapCPU, barycenterCPU, predicateCPU, 6 ))
  {
    deleteVector( mapGPU );
    deleteVector( mapCPU );
    return 0;
  }
  // return 0;

  QApplication application(argc,argv);
  DGtal::Viewer3D<> viewer;
  viewer.show();

  convertCPUtoGPU predicateGPU2(barycenterGPU, barycenterGPU);
  convertCPUtoGPU predicateCPU2(barycenterGPU, barycenterCPU);
  std::cout << "Barycenter of GPU file : {" << barycenterGPU->x << "," << barycenterGPU->y << "," << barycenterGPU->z << "}" << std::endl;
  std::cout << "Barycenter of CPU file : {" << barycenterCPU->x << "," << barycenterCPU->y << "," << barycenterCPU->z << "}" << std::endl;
  std::cout << "Offset : {" << predicateGPU2.getOffset().x << "," << predicateGPU2.getOffset().y << "," << predicateGPU2.getOffset().z << "}" << std::endl;
  std::cout << "Offset : {" << predicateCPU2.getOffset().x << "," << predicateCPU2.getOffset().y << "," << predicateCPU2.getOffset().z << "}" << std::endl;

  viewer.setFillColor(DGtal::Color::Red);
  showFile( mapGPU, viewer, predicateGPU2, radiusSphere );
  viewer.setFillColor(DGtal::Color::Green);
  showFile( mapCPU, viewer, predicateCPU2, radiusSphere );

  viewer << DGtal::Viewer3D<>::updateDisplay;

  delete barycenterGPU;
  delete barycenterCPU;
  deleteVector( mapGPU );
  deleteVector( mapCPU );

  return application.exec();
}
