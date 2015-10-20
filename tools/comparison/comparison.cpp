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
  Position& operator= (const Position& p)
  {
    x = p.x;
    y = p.y;
    z = p.z;

    return *this;
  }
  Position& operator+= (const Position& p)
  {
    x += p.x;
    y += p.y;
    z += p.z;

    return *this;
  }
  Position& operator/= (const int& v)
  {
    x /= v;
    y /= v;
    z /= v;

    return *this;
  }
  Position operator-( const Position& p) const
  {
    return Position(x-p.x, y-p.y, z-p.z);
  }
  Position operator+( const Position& p) const
  {
    return Position(x+p.x, y+p.y, z+p.z);
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

class convertCPUtoGPU : std::unary_function <Position, Position>
{
public:
  convertCPUtoGPU( Position* barycenterGPU, Position* barycenterCPU )
  {
    offset = *barycenterGPU - *barycenterCPU;
  }

  Position operator() (const Position v) const
  {
    return v + offset;
  }

  Position getOffset() const
  {
    return offset;
  }
private:
  Position offset;
};

template< typename Predicate >
bool loadFile(  const std::string& filename,
                std::vector< std::pair<Position*, Curvatures*> >& results,
                Position* barycenter,
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

      *barycenter += *p;

      Curvatures *c = new Curvatures();
      c->mean = std::atof(words[3].c_str());
      c->k1   = std::atof(words[4].c_str());
      c->k2   = std::atof(words[5].c_str());

      results.push_back( std::pair<Position*,Curvatures*>(p,c) );
    }
    *barycenter /= results.size();
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

template< typename Predicate >
bool normalizeCPU( const std::vector< std::pair<Position*, Curvatures*> >& mapCPU,
                   const std::vector< std::pair<Position*, Curvatures*> >& mapGPU,
                   std::vector< std::pair<Position*, Curvatures*> >& mapCPUnormalized,
                   const Predicate& predicate)
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
      double CPUx = predicate( *elemCPU.first ).x;
      double CPUy = predicate( *elemCPU.first ).y;
      double CPUz = predicate( *elemCPU.first ).z;

      double current_distance = std::sqrt((GPUx - CPUx)*(GPUx - CPUx)
                                        + (GPUy - CPUy)*(GPUy - CPUy)
                                        + (GPUz - CPUz)*(GPUz - CPUz));

      if( minimal_distance > current_distance )
      {
        minimal_distance = current_distance;
        minimal_position = elemCPU.first;
        minimal_positionCurvatures = elemCPU.second;
      }
    }

    if( minimal_distance == std::numeric_limits<double>::max() )
    {
      std::cout << "ERROR: No point was found. Are you sure that your files aren't empty ?" << std::endl;
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
                        std::vector< Curvatures* >& mapErrors )
{
  if( mapGPU.size() != mapCPUnormalized.size())
  {
    std::cout << "ERROR: mapGPU and mapCPUnormalized hasn't the same size...";
    std::cout << "(" << mapGPU.size() << ") and (" << mapCPUnormalized.size() << ")"<< std::endl;
    std::cout << "Leaving now..." << std::endl;
    return 0;
  }

  double error_mean_l1 = 0.0;
  double error_k1_l1 = 0.0;
  double error_k2_l1 = 0.0;

  double error_mean_l2 = 0.0;
  double error_k1_l2 = 0.0;
  double error_k2_l2 = 0.0;

  double error_mean_loo = 0.0;
  double error_k1_loo = 0.0;
  double error_k2_loo = 0.0;

  for( uint i = 0; i < mapGPU.size(); ++i )
  {
    //// l_1
    {
      error_mean_l1 += std::abs( mapGPU[i].second->mean - mapCPUnormalized[i].second->mean );
      error_k1_l1 += std::abs( mapGPU[i].second->k1 - mapCPUnormalized[i].second->k1 );
      error_k2_l1 += std::abs( mapGPU[i].second->k2 - mapCPUnormalized[i].second->k2 );
    }

    //// l_2
    {
      error_mean_l2 += std::abs( mapGPU[i].second->mean - mapCPUnormalized[i].second->mean ) * std::abs( mapGPU[i].second->mean - mapCPUnormalized[i].second->mean );
      error_k1_l2 += std::abs( mapGPU[i].second->k1 - mapCPUnormalized[i].second->k1 ) * std::abs( mapGPU[i].second->k1 - mapCPUnormalized[i].second->k1 );
      error_k2_l2 += std::abs( mapGPU[i].second->k2 - mapCPUnormalized[i].second->k2 ) * std::abs( mapGPU[i].second->k2 - mapCPUnormalized[i].second->k2 );
    }

    //// l_infty
    {
      if( error_mean_loo < std::abs( mapGPU[i].second->mean - mapCPUnormalized[i].second->mean ) )
      {
        error_mean_loo = std::abs( mapGPU[i].second->mean - mapCPUnormalized[i].second->mean );
      }
      if( error_k1_loo < std::abs( mapGPU[i].second->k1 - mapCPUnormalized[i].second->k1 ) )
      {
        error_k1_loo = std::abs( mapGPU[i].second->k1 - mapCPUnormalized[i].second->k1 );
      }
      if( error_k2_loo < std::abs( mapGPU[i].second->k2 - mapCPUnormalized[i].second->k2 ) )
      {
        error_k2_loo = std::abs( mapGPU[i].second->k2 - mapCPUnormalized[i].second->k2 );
      }
    }
  }

  //// l_1
  {
    error_mean_l1 /= (double)mapGPU.size();
    error_k1_l1 /= (double)mapGPU.size();
    error_k2_l1 /= (double)mapGPU.size();
  }

  //// l_2
  {
    error_mean_l2 = std::sqrt(error_mean_l2) / (double)mapGPU.size();
    error_k1_l2 = std::sqrt(error_k1_l2) / (double)mapGPU.size();
    error_k2_l2 = std::sqrt(error_k2_l2) / (double)mapGPU.size();
  }

  std::cout << "# ERROR\t l1\t l2\t l\\inty" << std::endl;
  std::cout << "mean:\t " << error_mean_l1 << "\t " << error_mean_l2 << "\t " << error_mean_loo << std::endl;
  std::cout << "k1:\t " << error_k1_l1 << "\t " << error_k1_l2 << "\t " << error_k1_loo << std::endl;
  std::cout << "k2:\t " << error_k2_l1 << "\t " << error_k2_l2 << "\t " << error_k2_loo << std::endl;

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
      std::cout << "Usage: " << argv[0] << "<fileGPU> <fileCPU>" << std::endl;
      std::cout << "       - computes the difference of results between the" << std::endl;
      std::cout << "         CPU version of the estimator and the GPU version" << std::endl;
      std::cout << "         GPU file : positions are between [0;size]." << std::endl;
      std::cout << "         CPU file : positions are between [0;2*size]" << std::endl;
      std::cout << "                                       or [-size;size]." << std::endl;
      std::cout << "Example:" << std::endl;
      std::cout << argv[ 0 ] << " file1.txt file2.txt" << std::endl;
      return 0;
    }
  std::string fileGPU = argc > 1 ? std::string( argv[ 1 ] ) : "file1.txt";
  std::string fileCPU = argc > 2 ? std::string( argv[ 2 ] ) : "file2.txt";

  //// Variables initialization
  std::vector< std::pair<Position*, Curvatures*> > mapGPU;
  std::vector< std::pair<Position*, Curvatures*> > mapCPU;
  std::vector< std::pair<Position*, Curvatures*> > mapCPUnormalized;
  std::vector< Curvatures* > mapErrors;
  convertGPUtoKhalimsky predicateGPU;
  convertCPUtoKhalimsky predicateCPU;
  Position* barycenterGPU = new Position();
  Position* barycenterCPU = new Position();

  //// Loading files
  if( !loadFile( fileGPU, mapGPU, barycenterGPU, predicateGPU ))
  {
    deleteVector( mapGPU );
    return 0;
  }
  if( !loadFile( fileCPU, mapCPU, barycenterCPU, predicateCPU ))
  {
    deleteVector( mapGPU );
    deleteVector( mapCPU );
    return 0;
  }


  //// Normalize inputs
  convertCPUtoGPU predicateCPUnormalized(barycenterGPU, barycenterCPU);
  std::cout << "Barycenter of GPU file : {" << barycenterGPU->x << "," << barycenterGPU->y << "," << barycenterGPU->z << "}" << std::endl;
  std::cout << "Barycenter of CPU file : {" << barycenterCPU->x << "," << barycenterCPU->y << "," << barycenterCPU->z << "}" << std::endl;
  std::cout << "Offset : {" << predicateCPUnormalized.getOffset().x << "," << predicateCPUnormalized.getOffset().y << "," << predicateCPUnormalized.getOffset().z << "}" << std::endl;
  if( !normalizeCPU( mapCPU, mapGPU, mapCPUnormalized, predicateCPUnormalized ))
  {
    deleteVector( mapGPU );
    deleteVector( mapCPU );
    return 0;
  }
  deleteVector( mapCPU );

  //// Computation some statistics.
  computeDifference( mapGPU, mapCPUnormalized, mapErrors );

  //// Releasing allocated memory
  deleteVector( mapGPU );
  deleteVector( mapCPUnormalized );
  delete barycenterGPU;
  delete barycenterCPU;
  return 0;
}
