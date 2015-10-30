/*
 * Copyright 2015 
 * Hélène Perrier <helene.perrier@liris.cnrs.fr>
 * Jérémy Levallois <jeremy.levallois@liris.cnrs.fr>
 * David Coeurjolly <david.coeurjolly@liris.cnrs.fr>
 * Jacques-Olivier Lachaud <jacques-olivier.lachaud@univ-savoie.fr>
 * Jean-Philippe Farrugia <jean-philippe.farrugia@liris.cnrs.fr>
 * Jean-Claude Iehl <jean-claude.iehl@liris.cnrs.fr>
 * 
 * This file is part of ICTV.
 * 
 * ICTV is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * ICTV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ICTV.  If not, see <http://www.gnu.org/licenses/>
 */

#include <iostream>
#include <cstdlib>
#include <boost/algorithm/string.hpp>
#include <map>
#include <vector>
#include <fstream>
#include <cmath>
#include <limits>

#include "helpers.h"

#define NBDATA 4 /// x y z H k1 k2

template< typename Predicate >
bool loadFile2(  const std::string& filename,
                std::vector< std::pair<Position*, Value> >& results,
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
      if(line[0] == '#' || line[0] == 'N' || line.size() < 3)
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

      Value c = std::atof(words[3].c_str());

      results.push_back( std::pair<Position*,Value>(p,c) );
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


bool writeVector( const std::vector< std::pair<Position*, Value> >& input,
                  std::vector< std::pair<Position*, Curvatures*> >& output,
                  const uint position)
{
  if(output.size() != input.size() && output.size() != 0)
  {
    std::cout << "ERROR: size doesn't match : " << input.size() << " vs " << output.size() << std::endl;
    std::cout << "Leaving now..." << std::endl;
    return false;
  }
  if( output.size() != 0)
  {
    for(uint i = 0; i < input.size(); ++i)
    {
      if( position == 0 )
        output[i].second->mean = input[i].second;
      if( position == 1 )
        output[i].second->k1 = input[i].second;
      if( position == 2 )
        output[i].second->k2 = input[i].second;
    }
  }
  else
  {
    for(uint i = 0; i < input.size(); ++i)
    {
      Position *p = new Position();
      p->x = input[i].first->x;
      p->y = input[i].first->y;
      p->z = input[i].first->z;
      Curvatures *c = new Curvatures();

      if( position == 0 )
        c->mean = input[i].second;
      if( position == 1 )
        c->k1 = input[i].second;
      if( position == 2 )
        c->k2 = input[i].second;

    output.push_back(std::pair<Position*,Curvatures*>(p,c));
    }

  }
}

void writeFile( const std::string& fileOutput,
                const std::vector< std::pair< Position*, Curvatures*> > & v_export )
{
  std::ofstream myfile;
  myfile.open (fileOutput.c_str());
  for(uint i = 0; i < v_export.size(); ++i)
  {
    myfile << v_export[i].first->x << " ";
    myfile << v_export[i].first->y << " ";
    myfile << v_export[i].first->z << " ";
    myfile << v_export[i].second->mean << " ";
    myfile << v_export[i].second->k1 << " ";
    myfile << v_export[i].second->k2 << std::endl;
  }
  myfile.close();
}

void deleteVector2( std::vector< std::pair<Position*, Value> >& _map )
{
  for(  std::vector< std::pair<Position*, Value> >::iterator it=_map.begin();
        it!=_map.end(); ++it )
  {
    delete (it->first);
  }

  _map.clear();
}

int main( int argc, char** argv )
{
  //// User's choice
  if ( argc == 1 )
    {
      std::cout << "Usage: " << argv[0] << "<output> <inputFile1> <inputFile2> ..." << std::endl;
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
  std::string fileOutput = argc > 1 ? std::string( argv[ 1 ] ) : "file1.txt";
  std::string fileInput = argc > 2 ? std::string( argv[ 2 ] ) : "file2.txt";
  convertCPUtoKhalimsky predicateCPU;
  std::vector< std::pair<Position*, Curvatures*> > v_export;
  std::vector< std::pair<Position*, Value> > v_temp;
  loadFile2( fileInput, v_temp, predicateCPU );
  writeVector(v_temp, v_export, 0 );
  deleteVector2( v_temp );

  for(int i = 3; i < argc; ++i)
  {
    fileInput = std::string( argv[ i ] );
    loadFile2( fileInput, v_temp, predicateCPU );
    writeVector(v_temp, v_export, i-2);
    deleteVector2( v_temp );
  }

  writeFile( fileOutput, v_export );
  deleteVector( v_export );

  return 0;
}
