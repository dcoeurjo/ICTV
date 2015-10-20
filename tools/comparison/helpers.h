#pragma once

#if defined(helpers_RECURSES)
#error Recursive header files inclusion detected in helpers.h
#else // defined(helpers_RECURSES)
/** Prevents recursive inclusion of headers. */
#define helpers_RECURSES

#if !defined helpers_h
/** Prevents repeated inclusion of headers. */
#define helpers_h

#include <iostream>

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
  Position& operator+= (const int& v)
  {
    x += v;
    y += v;
    z += v;

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
                const Predicate& predicate,
                const uint NBDATA )
{
  std::ifstream file( filename.c_str() );
  std::string line;

  if( file.is_open())
  {
    std::cout << "Collecting data from file " << filename << " ..."<< std::endl;
    while( getline(file,line) )
    {
      /// Skip if the line contains a # char, or N, or any line too small (avoid the new line a end of file)
      if(line[0] == '#' || line[0] == 'N' )//|| line.size() < NBDATA)
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
      if( NBDATA > 3 )
      {
        c->mean = std::atof(words[3].c_str());
        c->k1   = std::atof(words[4].c_str());
        c->k2   = std::atof(words[5].c_str());
      }

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


#endif // !defined helpers_h

#undef helpers_RECURSES
#endif // else defined(helpers_RECURSES)
