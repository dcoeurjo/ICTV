#include "DataLoader.h"

#include "GL/GLTexture.h"

void DataLoader::loadData32BGpu()
{
	glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_DENSITY]);
	glBindTexture(GL_TEXTURE_3D, Parameters::getInstance()->g_textures[TEXTURE_DENSITY]);
	
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, sizex, sizey, sizez, 0, GL_RED, GL_FLOAT, data);
        glGenerateMipmap(GL_TEXTURE_3D);
	
	glBindTexture(GL_TEXTURE_3D, 0);
}

void DataLoader::loadData8BGpu()
{
	glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_DENSITY]);
	glBindTexture(GL_TEXTURE_3D, Parameters::getInstance()->g_textures[TEXTURE_DENSITY]);
	
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, sizex, sizey, sizez, 0, GL_RED, GL_FLOAT, data_char);
        glGenerateMipmap(GL_TEXTURE_3D);
	
	glBindTexture(GL_TEXTURE_3D, 0);
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
	
        /*char* line1 = (char*)malloc(sizeof(char)*10);
        fread(line1, sizeof(unsigned char), 8, fd);
        
        printf("%s\n", line1);*/
        
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
