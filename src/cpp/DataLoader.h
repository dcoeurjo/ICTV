#ifndef DATALOADER_H_
#define DATALOADER_H_

#include "Parameters.h"

#include <cstdlib>

class DataLoader
{
protected:
	unsigned int sizex;
	unsigned int sizey;
	unsigned int sizez;
	
	float* data;
	unsigned char* data_char;
public:
	virtual void loadFile(char* file) { sizex = sizey = sizez = 0; }
	virtual float* get32BData() { return data; }
	virtual unsigned char* get8BData() { return data_char; }
	
	virtual void loadData32BGpu();
	virtual void loadData8BGpu();
	
	virtual void releaseData() { free(data); }
};

class DataTerrain : public DataLoader
{
public:	
	virtual float* get32BData() { return data; }
	virtual unsigned char* get8BData() { return data_char; }
	virtual void loadFile(char* file);
};

class DataRaw : public DataLoader
{
public:	
	virtual float* get32BData() { return data; }
	virtual unsigned char* get8BData() { return data_char; }
	virtual void loadFile(char* file);
};


#endif