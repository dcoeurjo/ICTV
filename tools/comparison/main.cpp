#include <iostream>
#include <cstdlib>
#include <boost/algorithm/string.hpp>
#include <map>
#include <vector>
#include <fstream>
#include <cmath>

#define NBDATA 6 /// x y z H k1 k2

typedef unsigned int uint;
typedef float Value;

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
bool loadFile(  const std::string& filename, std::map<Position,
                Curvatures>& results, const Predicate& predicate)
{
  std::ifstream file( filename.c_str() );
  std::string line;

  if( file.is_open())
  {
    std::cout << "Collecting data from file ..." << filename << std::endl;
    while( getline(file,line) )
    {
      /// Skip if the line contains a # char or N
      if(line[0] == '#' || line[0] == 'N')
      {
        continue;
      }

      std::vector<std::string> words;
      boost::split(words, line, boost::is_any_of(" \t"), boost::token_compress_on);
      if(words.size() < NBDATA)
      {
        std::cout << "I've got a wrong number of data on this line..." << std::endl;
        std::cout << "        ";
        for( uint i_word = 0; i_word < words.size(); ++i_word )
        {
            std::cout << words[i_word] << " ##### ";
        }
        std::cout << std::endl;
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

      results[*p] = *c;

      std::cout << p->x << " " << p->y << " " << p->z << " ";
      std::cout << c->mean << " " << c->k1 << " " << c->k2 << std::endl;;
    }
    std::cout << "... done" << filename << std::endl;
  }
  else
  {
    std::cout << "The file " << filename << " can't be opened. Is the file exists ?" << std::endl;
    return false;
  }
  return true;
}

int main( int argc, char** argv )
{
  //// User's choice
  if ( argc == 1 )
    {
      std::cout << "Usage: " << argv[0] << "<fileGPU> <fileCPU> <size>" << std::endl;
      std::cout << "       - computes the difference of results between the" << std::endl;
      std::cout << "         CPU version of the estimator and the GPU version" << std::endl;
      std::cout << "         GPU file : positions are between [0;size]." << std::endl;
      std::cout << "         CPU file : positions are between [0;2*size]" << std::endl;
      std::cout << "                                       or [-size;size]." << std::endl;
      std::cout << "Example:" << std::endl;
      std::cout << argv[ 0 ] << " file1.txt file2.txt 64" << std::endl;
      return 0;
    }
  std::string fileGPU = argc > 1 ? std::string( argv[ 1 ] ) : "file1.txt";
  std::string fileCPU = argc > 2 ? std::string( argv[ 2 ] ) : "file2.txt";
  uint size = argc > 3 ? std::atoi( argv[ 3 ] ) : 64;

  //// Variables initialization
  std::map<Position, Curvatures> mapGPU;
  std::map<Position, Curvatures> mapCPU;
  convertGPUtoKhalimsky predicateGPU;
  convertCPUtoKhalimsky predicateCPU;

  loadFile( fileGPU, mapGPU, predicateGPU );
  loadFile( fileCPU, mapCPU, predicateCPU );



  return 0;
}
