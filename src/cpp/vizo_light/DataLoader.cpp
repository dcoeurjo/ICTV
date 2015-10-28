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

#include "DataLoader.h"

#include "GL/GLTexture.h"

#include <cstring>

const GLenum MIN_FILT = GL_LINEAR_MIPMAP_LINEAR;
const GLenum MAG_FILT = GL_LINEAR;
//const GLenum MIN_FILT = GL_NEAREST_MIPMAP_NEAREST;
//const GLenum MAG_FILT = GL_NEAREST;

bool mymipmap = false;
bool novolumenomoments = true;

void DataLoader::loadData32BGpu()
{
	glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_DENSITY]);
	glBindTexture(GL_TEXTURE_3D, Parameters::getInstance()->g_textures[TEXTURE_DENSITY]);
	
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, MAG_FILT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, MIN_FILT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, sizex, sizey, sizez, 0, GL_RED, GL_FLOAT, data);
        glGenerateMipmap(GL_TEXTURE_3D);
	
	glBindTexture(GL_TEXTURE_3D, 0);
}

void DataLoader::loadData8BGpu()
{
	glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_DENSITY]);
	glBindTexture(GL_TEXTURE_3D, Parameters::getInstance()->g_textures[TEXTURE_DENSITY]);
	
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, MAG_FILT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, MIN_FILT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, sizex, sizey, sizez, 0, GL_RED, GL_UNSIGNED_BYTE, data_char);
        glGenerateMipmap(GL_TEXTURE_3D);
	
	glBindTexture(GL_TEXTURE_3D, 0);
}

void loadNxtMipmap(int size, float* texture, float** miptexture)
{
	int sizemip = ceil(size/2.0);
	printf("sizemip %d\n", sizemip);
	
	*miptexture = (float*)malloc(sizeof(float)*sizemip*sizemip*sizemip*3);
	memset(*miptexture, 0, sizeof(float)*sizemip*sizemip*sizemip*3);
	
	int nb = 0;
	for(int k=0; k<size; k++)
	for(int j=0; j<size; j++)
	for(int i=0; i<size; i++)
	{
		int mip_i = i/2;
		int mip_j = j/2;
		int mip_k = k/2;
		
		int off = (mip_k*sizemip+mip_j)*sizemip+mip_i;
		
		(*miptexture)[off*3] += texture[nb*3];
		(*miptexture)[off*3+1] += texture[nb*3+1];
		(*miptexture)[off*3+2] += texture[nb*3+2];
		
		nb++;
	}
}

void DataLoader::loadx2y2z2()
{
	float* mmt;
	long unsigned int alloc_size = sizex*sizey*sizez*3;
	mmt = (float*)malloc(sizeof(float)*alloc_size);
	int nb = 0;
	for(unsigned int k=0; k<sizez; k++)
	for(unsigned int j=0; j<sizey; j++)
	for(unsigned int i=0; i<sizex; i++)
	{
		float norm_i = i;
		float norm_j = j;
		float norm_k = k;
		
		mmt[nb*3] = norm_i*norm_i;
		mmt[nb*3 + 1] = norm_j*norm_j;
		mmt[nb*3 + 2] = norm_k*norm_k;
		
		if (novolumenomoments)
		{
			mmt[nb*3] *= data[nb];
			mmt[nb*3 + 1] *= data[nb];
			mmt[nb*3 + 2] *= data[nb];
		}
		nb++;
	}
	
	glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_X2Y2Z2]);
	glBindTexture(GL_TEXTURE_3D, Parameters::getInstance()->g_textures[TEXTURE_X2Y2Z2]);
	
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, MAG_FILT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, MIN_FILT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	int lvl = 0;
	glTexImage3D(GL_TEXTURE_3D, lvl++, GL_RGB32F, sizex, sizey, sizez, 0, GL_RGB, GL_FLOAT, mmt);
		
	
	if (mymipmap)
	{
		int size = sizex;
		float *mip0, *mip1;
		mip1 = mmt;
		while(size > 1)
		{
			loadNxtMipmap(size, mip1, &mip0);
			size = ceil(size/2.0);
			glTexImage3D(GL_TEXTURE_3D, lvl++, GL_RGB32F, size, size, size, 0, GL_RGB, GL_FLOAT, mip0);
			free(mip1);
			mip1 = mip0;
		}
		free(mip0);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, lvl-1);
	}
	else
	{
		glGenerateMipmap(GL_TEXTURE_3D);
		free(mmt);
	}
	
	glBindTexture(GL_TEXTURE_3D, 0);
}


void DataLoader::loadxyyzxz()
{
	float* mmt;
	long unsigned int alloc_size = sizex*sizey*sizez*3;
	mmt = (float*)malloc(sizeof(float)*alloc_size);
	int nb = 0;
	for(unsigned int k=0; k<sizez; k++)
	for(unsigned int j=0; j<sizey; j++)
	for(unsigned int i=0; i<sizex; i++)
	{
		float norm_i = i;
		float norm_j = j;
		float norm_k = k;
		
		mmt[nb*3] = norm_i*norm_j;
		mmt[nb*3 + 1] = norm_j*norm_k;
		mmt[nb*3 + 2] = norm_k*norm_i;
		
		if (novolumenomoments)
		{
			mmt[nb*3] *= data[nb];
			mmt[nb*3 + 1] *= data[nb];
			mmt[nb*3 + 2] *= data[nb];
		}
		nb++;
	}
	
	glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_XY_YZ_XZ]);
	glBindTexture(GL_TEXTURE_3D, Parameters::getInstance()->g_textures[TEXTURE_XY_YZ_XZ]);
	
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, MAG_FILT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, MIN_FILT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	int lvl = 0;
	glTexImage3D(GL_TEXTURE_3D, lvl++, GL_RGB32F, sizex, sizey, sizez, 0, GL_RGB, GL_FLOAT, mmt);
		
		
	if (mymipmap)
	{
		int size = sizex;
		float *mip0, *mip1;
		mip1 = mmt;
		while(size > 1)
		{
			loadNxtMipmap(size, mip1, &mip0);
			size = ceil(size/2.0);
			glTexImage3D(GL_TEXTURE_3D, lvl++, GL_RGB32F, size, size, size, 0, GL_RGB, GL_FLOAT, mip0);
			free(mip1);
			mip1 = mip0;
		}
		free(mip0);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, lvl-1);
	}
	else
	{
		glGenerateMipmap(GL_TEXTURE_3D);
		free(mmt);
	}
	
	glBindTexture(GL_TEXTURE_3D, 0);
	
	//free(mmt);
}

void DataLoader::loadxyz()
{
	float* mmt;
	long unsigned int alloc_size = sizex*sizey*sizez*3;
	mmt = (float*)malloc(sizeof(float)*alloc_size);
	int nb = 0;
	for(unsigned int k=0; k<sizez; k++)
	for(unsigned int j=0; j<sizey; j++)
	for(unsigned int i=0; i<sizex; i++)
	{
		float norm_i = i;
		float norm_j = j;
		float norm_k = k;
		
		mmt[nb*3] = norm_i;
		mmt[nb*3 + 1] = norm_j;
		mmt[nb*3 + 2] = norm_k;
		
		if (novolumenomoments)
		{
			mmt[nb*3] *= data[nb];
			mmt[nb*3 + 1] *= data[nb];
			mmt[nb*3 + 2] *= data[nb];
		}
		
		nb++;
	}
	
	glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_XYZ]);
	glBindTexture(GL_TEXTURE_3D, Parameters::getInstance()->g_textures[TEXTURE_XYZ]);
	
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, MAG_FILT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, MIN_FILT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	int lvl = 0;
	glTexImage3D(GL_TEXTURE_3D, lvl++, GL_RGB32F, sizex, sizey, sizez, 0, GL_RGB, GL_FLOAT, mmt);
	
	if(mymipmap)
	{
		int size = sizex;
		float *mip0, *mip1;
		mip1 = mmt;
		while(size > 1)
		{
			loadNxtMipmap(size, mip1, &mip0);
			size = ceil(size/2.0);
			glTexImage3D(GL_TEXTURE_3D, lvl++, GL_RGB32F, size, size, size, 0, GL_RGB, GL_FLOAT, mip0);
			free(mip1);
			mip1 = mip0;
		}
		free(mip0);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, lvl-1);
	}
	else
	{
		glGenerateMipmap(GL_TEXTURE_3D);
		free(mmt);
	}
	
	glBindTexture(GL_TEXTURE_3D, 0);
	
	//free(mmt);
}


unsigned long int f(unsigned int sf, unsigned long int x, unsigned long int y, unsigned long int z)
{
	return ((z*sf) + x)*sf + y;
}

//Files provided by Eric
void DataTerrain::loadFile(char* file)
{
	printf("Loading Eric's file as float %s ... ", file); fflush(stdout);

	FILE* fd = fopen(file, "r");
	if (fd == NULL)
	{
		perror("Error at file opening");
		return;
	}
	
        int cu[3];
        unsigned long int r = fread(cu, sizeof(int), 3, fd);
     
        unsigned int sf = 512;
        sizex = sf;
        sizey = sf;
        sizez = sf;
	
        unsigned long int total = sizex * sizey * sizez;
        data = (float*) malloc(sizeof(float)*total);
        data_char = (unsigned char*) malloc(sizeof(unsigned char)*total);
      
        r = fread(data_char, sizeof(unsigned char), total, fd);
	if (r != total)
		printf("Read only %lu values\n", r);
	
	fclose(fd);
	
	unsigned long int i = 0;
	for(unsigned long int x=0; x < sf; x++)
	for(unsigned long int y=0; y < sf; y++)
	for(unsigned long int z=0; z < sf; z++)
		data[i++] = ((float)data_char[f(sf, x, y, z)] / 255.0) - 0.5;

	printf("Complete\n");
}

//Files .raw provided by http://www.tc18.org/code_data_set/3D_images.php
void DataRaw::loadFile(char* file)
{
	printf("Loading .raw file %s (size %d^3) ...\n", file, sizex);
	
	FILE* fd = fopen(file, "r");
	if (fd == NULL)
	{
		perror("Error at file opening");
		return;
	}

        
	unsigned long int total = sizex * sizey * sizez;
    data = (float*) malloc(sizeof(float)*total);
    data_char = (unsigned char*) malloc(sizeof(unsigned char)*total);
        
    fread(data_char, sizeof(unsigned char), total, fd);
	fclose(fd);

	unsigned char min = data_char[0];
	unsigned char max = data_char[0];

	unsigned long int i = 0;
	for(unsigned long int x=0; x < sizex; x++)
	for(unsigned long int y=0; y < sizey; y++)
	for(unsigned long int z=0; z < sizez; z++)
	{
		if (data_char[i]<min)
			min = data_char[i];

		if (data_char[i]>max)
			max = data_char[i];

		i++;
	}
	
	if (min != 0)
		printf("ERROR min value != 0; normalization will fail");
	printf("Max %d Min %d\n", max, min);

	i = 0;
	for(unsigned long int x=0; x < sizex; x++)
	for(unsigned long int y=0; y < sizey; y++)
	for(unsigned long int z=0; z < sizez; z++)
	{
		if (data_char[i] == 0)
			data[i] = 0;
		else
			data[i] = 1;

		//data[i] = ((float)data_char[i] / (float)max);
		i++;
	}

	Parameters::getInstance()->g_sizetex = sizex;

	printf("Complete\n");
}
