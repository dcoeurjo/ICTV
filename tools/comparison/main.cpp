#include <iostream>
#include <cstdlib>
#include <boost/algorithm/string.hpp>
#include <map>
#include <vector>
#include <fstream>
#include <cmath>
#include <limits>

#define NBDATA 6 /// x y z H k1 k2

typedef unsigned int uint;
typedef double Value;

struct Curvatures
{
  Value mean;
  Value k1;
  Value k2;
};
class Position
{
public:
  int x;
  int y;
  int z;

public:
  Position(){ x=0; y=0; z=0; }
  Position(int _x, int _y, int _z){ x=_x; y=_y; z=_z; }

  bool operator< (const Position& p) const
  {
    if ( x != p.x )
    {
      return x < p.x;
    }
    if ( y != p.y )
    {
      return y < p.y;
    }
    if ( z != p.z )
    {
      return z < p.z;
    }
    return false;
  }
  bool operator> (const Position& p) const
  {
    if( *this == p)
    {
      return false;
    }
    return !(*this < p);
  }
  bool operator== (const Position& p) const
  {
    return x == p.x && y == p.y && z == p.y;
  }
  bool operator!= (const Position& p) const
  {
    return !(*this == p);
  }

  Position operator-( const Position& p) const
  {
    return Position(x-p.x, y-p.y, z-p.z);
  }
};

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
                std::vector< std::pair<Position*, Curvatures*> >& results,
                const Predicate& predicate )
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

      Position *p = new Position();
      p->x = predicate( std::atof(words[0].c_str()));
      p->y = predicate( std::atof(words[1].c_str()));
      p->z = predicate( std::atof(words[2].c_str()));

      Curvatures *c = new Curvatures();
      c->mean = std::atof(words[3].c_str());
      c->k1   = std::atof(words[4].c_str());
      c->k2   = std::atof(words[5].c_str());

      results.push_back( std::pair<Position*,Curvatures*>(p,c) );
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

bool normalizeCPU( const std::vector< std::pair<Position*, Curvatures*> >& mapCPU,
                   const std::vector< std::pair<Position*, Curvatures*> >& mapGPU,
                   std::vector< std::pair<Position*, Curvatures*> >& mapCPUnormalized )
{
  for( std::pair<Position*, Curvatures*> elemGPU : mapGPU )
  {
    double minimal_distance = std::numeric_limits<double>::max();
    Position* minimal_position;
    Curvatures* minimal_positionCurvatures;
    for( std::pair<Position*, Curvatures*> elemCPU : mapCPU )
    {
      double GPUx = elemGPU.first->x;
      double GPUy = elemGPU.first->y;
      double GPUz = elemGPU.first->z;
      double CPUx = elemCPU.first->x;
      double CPUy = elemCPU.first->y;
      double CPUz = elemCPU.first->z;

      double current_distance = std::sqrt((GPUx - CPUx)*(GPUx - CPUx)
                                        + (GPUy - CPUy)*(GPUy - CPUy)
                                        + (GPUz - CPUz)*(GPUz - CPUz));

      if( minimal_distance > current_distance )
      {
        minimal_distance = current_distance;
        minimal_position = elemCPU.first;
        minimal_positionCurvatures = elemCPU.second;
        // if( minimal_distance <= 0 )
        // {
        //   std::cout << "************************"<<std::endl;
        //   std::cout << "TEST {" << elemGPU.first->x << "," << elemGPU.first->y << "," << elemGPU.first->z << "} : "
        //             << "(" << elemGPU.second->mean << "," << elemGPU.second->k1 << "," << elemGPU.second->k2 << ")" << std::endl;
        //   std::cout << "TEST {" << elemCPU.first->x << "," << elemCPU.first->y << "," << elemCPU.first->z << "} : "
        //             << "(" << elemCPU.second->mean << "," << elemCPU.second->k1 << "," << elemCPU.second->k2 << ")" << std::endl;
        //   std::cout << "************************"<<std::endl;
        // }
      }
    }

    if( minimal_distance == std::numeric_limits<double>::max() )
    {
      std::cout << "ERROR: No point was found. Are you sure that your files aren't empty ?" << std::endl;
      std::cout << "Leaving now..." << std::endl;
      return false;
    }
    if( minimal_distance != 0 )
    {
      std::cout << "ERROR: The distance between {" << elemGPU.first->x << ","
                                            << elemGPU.first->y << ","
                                            << elemGPU.first->z << "} and {"
                                            << minimal_position->x << ","
                                            << minimal_position->y << ","
                                            << minimal_position->z
                << "} is greater than 1 (exactly " << std::sqrt(minimal_distance) << ")." << std::endl;
      std::cout << "Leaving now..." << std::endl;
      return false;
    }

    Position *copy_pos = new Position();
    Curvatures *copy_curv = new Curvatures();
    copy_pos->x = elemGPU.first->x;
    copy_pos->y = elemGPU.first->y;
    copy_pos->z = elemGPU.first->z;
    copy_curv->mean = minimal_positionCurvatures->mean;
    copy_curv->k1 = minimal_positionCurvatures->k1;
    copy_curv->k2 = minimal_positionCurvatures->k2;
    mapCPUnormalized.push_back( std::pair<Position*, Curvatures*>(copy_pos, copy_curv) );
  }
  return true;
}

bool computeDifference( const std::vector< std::pair<Position*, Curvatures*> >& mapGPU,
                        const std::vector< std::pair<Position*, Curvatures*> >& mapCPUnormalized,
                        const uint errorType,
                        std::vector< Curvatures* >& mapErrors )
{
  if( mapGPU.size() != mapCPUnormalized.size())
  {
    std::cout << "ERROR: mapGPU and mapCPUnormalized hasn't the same size...";
    std::cout << "(" << mapGPU.size() << ") and (" << mapCPUnormalized.size() << ")"<< std::endl;
    std::cout << "Leaving now..." << std::endl;
    return 0;
  }

  // std::cout << "TEST {" << mapGPU[4].first->x << "," << mapGPU[4].first->y << "," << mapGPU[4].first->z << "} : "
  //           << "(" << mapGPU[4].second->mean << "," << mapGPU[4].second->k1 << "," << mapGPU[4].second->k2 << ")" << std::endl;
  // std::cout << "TEST {" << mapCPUnormalized[4].first->x << "," << mapCPUnormalized[4].first->y << "," << mapCPUnormalized[4].first->z << "} : "
  //           << "(" << mapCPUnormalized[4].second->mean << "," << mapCPUnormalized[4].second->k1 << "," << mapCPUnormalized[4].second->k2 << ")" << std::endl;

  double error_mean = 0.0;
  double error_k1 = 0.0;
  double error_k2 = 0.0;

  for( uint i = 0; i < mapGPU.size(); ++i )
  {
    if( errorType == 1 ) /// l_1
    {
      error_mean += std::abs( mapGPU[i].second->mean - mapCPUnormalized[i].second->mean );
      error_k1 += std::abs( mapGPU[i].second->k1 - mapCPUnormalized[i].second->k1 );
      error_k2 += std::abs( mapGPU[i].second->k2 - mapCPUnormalized[i].second->k2 );
    }
    else if( errorType == 2 ) /// l_2
    {
      error_mean += std::abs( mapGPU[i].second->mean - mapCPUnormalized[i].second->mean ) * std::abs( mapGPU[i].second->mean - mapCPUnormalized[i].second->mean );
      error_k1 += std::abs( mapGPU[i].second->k1 - mapCPUnormalized[i].second->k1 ) * std::abs( mapGPU[i].second->k1 - mapCPUnormalized[i].second->k1 );
      error_k2 += std::abs( mapGPU[i].second->k2 - mapCPUnormalized[i].second->k2 ) * std::abs( mapGPU[i].second->k2 - mapCPUnormalized[i].second->k2 );
    }
    else if( errorType == 3 ) /// l_infty
    {
      if( error_mean < std::abs( mapGPU[i].second->mean - mapCPUnormalized[i].second->mean ) )
      {
        error_mean = std::abs( mapGPU[i].second->mean - mapCPUnormalized[i].second->mean );
      }
      if( error_k1 < std::abs( mapGPU[i].second->k1 - mapCPUnormalized[i].second->k1 ) )
      {
        error_k1 = std::abs( mapGPU[i].second->k1 - mapCPUnormalized[i].second->k1 );
      }
      if( error_k2 < std::abs( mapGPU[i].second->k2 - mapCPUnormalized[i].second->k2 ) )
      {
        error_k2 = std::abs( mapGPU[i].second->k2 - mapCPUnormalized[i].second->k2 );
        // if(error_k2 > 0.000001)
        // {
        //   std::cout << "ERROR:" << mapGPU[i].second->k2 << " " << mapCPUnormalized[i].second->k2 << std::endl;
        //   std::cout << "ERROR:" << mapGPU[i].first->x << " " << mapCPUnormalized[i].first->x << std::endl;
        //   std::cout << "ERROR:" << mapGPU[i].first->y << " " << mapCPUnormalized[i].first->y << std::endl;
        //   std::cout << "ERROR:" << mapGPU[i].first->z << " " << mapCPUnormalized[i].first->z << std::endl;
        // }
        // std::cout << "TEST {" << mapGPU[4].first->x << "," << mapGPU[4].first->y << "," << mapGPU[4].first->z << "} : "
        //           << "(" << mapGPU[4].second->mean << "," << mapGPU[4].second->k1 << "," << mapGPU[4].second->k2 << ")" << std::endl;
        // std::cout << "TEST {" << mapCPUnormalized[4].first->x << "," << mapCPUnormalized[4].first->y << "," << mapCPUnormalized[4].first->z << "} : "
        //           << "(" << mapCPUnormalized[4].second->mean << "," << mapCPUnormalized[4].second->k1 << "," << mapCPUnormalized[4].second->k2 << ")" << std::endl;
        // std::cout << "I is " << i << std::endl;
      }
    }
    else
    {
      std::cout << "ERROR: errorType isn't in {1,2,3} (aka l_1, l_2 and l_\\infty error)...";
      std::cout << "Leaving now..." << std::endl;
      return 0;
    }
  }
  if( errorType == 1 ) /// l_1
  {
    error_mean /= (double)mapGPU.size();
    error_k1 /= (double)mapGPU.size();
    error_k2 /= (double)mapGPU.size();
  }
  else if( errorType == 2 ) /// l_2
  {
    error_mean = std::sqrt(error_mean) / (double)mapGPU.size();
    error_k1 = std::sqrt(error_k1) / (double)mapGPU.size();
    error_k2 = std::sqrt(error_k2) / (double)mapGPU.size();
  }

  std::cout << "mean: " << error_mean << std::endl;
  std::cout << "k1: " << error_k1 << std::endl;
  std::cout << "k2: " << error_k2 << std::endl;

  return true;
}

void deleteVector( std::vector< std::pair<Position*, Curvatures*> >& _map )
{
  for(  std::vector< std::pair<Position*, Curvatures*> >::iterator it=_map.begin();
        it!=_map.end(); ++it )
  {
    delete (it->first);
    delete (it->second);
  }

  _map.clear();
}

int main( int argc, char** argv )
{
  //// User's choice
  if ( argc == 1 )
    {
      std::cout << "Usage: " << argv[0] << "<fileGPU> <fileCPU> <size> <errorType>" << std::endl;
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
  uint size = argc > 3 ? std::atoi( argv[ 3 ] ) : 64;
  uint errorType = argc > 4 ? std::atoi( argv[ 4 ] ) : 3;

  //// Variables initialization
  std::vector< std::pair<Position*, Curvatures*> > mapGPU;
  std::vector< std::pair<Position*, Curvatures*> > mapCPU;
  std::vector< std::pair<Position*, Curvatures*> > mapCPUnormalized;
  std::vector< Curvatures* > mapErrors;
  convertGPUtoKhalimsky predicateGPU;
  convertCPUtoKhalimsky predicateCPU;

  //// Loading files
  if( !loadFile( fileGPU, mapGPU, predicateGPU ))
  {
    deleteVector( mapGPU );
    return 0;
  }
  if( !loadFile( fileCPU, mapCPU, predicateGPU ))
  {
    deleteVector( mapGPU );
    deleteVector( mapCPU );
    return 0;
  }
  // std::cout << "TEST {" << mapGPU[4].first->x << "," << mapGPU[4].first->y << "," << mapGPU[4].first->z << "} : "
  //           << "(" << mapGPU[4].second->mean << "," << mapGPU[4].second->k1 << "," << mapGPU[4].second->k2 << ")" << std::endl;
  // std::cout << "TEST {" << mapCPU[4].first->x << "," << mapCPU[4].first->y << "," << mapCPU[4].first->z << "} : "
  //           << "(" << mapCPU[4].second->mean << "," << mapCPU[4].second->k1 << "," << mapCPU[4].second->k2 << ")" << std::endl;

  //// Normalize inputs
  if( !normalizeCPU( mapCPU, mapGPU, mapCPUnormalized ))
  {
    deleteVector( mapGPU );
    deleteVector( mapCPU );
    return 0;
  }
  // std::cout << "TEST {" << mapCPUnormalized[4].first->x << "," << mapCPUnormalized[4].first->y << "," << mapCPUnormalized[4].first->z << "} : "
  //           << "(" << mapCPUnormalized[4].second->mean << "," << mapCPUnormalized[4].second->k1 << "," << mapCPUnormalized[4].second->k2 << ")" << std::endl;
  deleteVector( mapCPU );

  // std::cout << "TEST {" << mapCPUnormalized[4].first->x << "," << mapCPUnormalized[4].first->y << "," << mapCPUnormalized[4].first->z << "} : "
  //           << "(" << mapCPUnormalized[4].second->mean << "," << mapCPUnormalized[4].second->k1 << "," << mapCPUnormalized[4].second->k2 << ")" << std::endl;
  // std::cout << "---------------------------------------" << std::endl;

  //// Computation some statistics.
  computeDifference( mapGPU, mapCPUnormalized, errorType, mapErrors );

  //// Releasing allocated memory
  deleteVector( mapGPU );
  deleteVector( mapCPUnormalized );
  return 0;
}
