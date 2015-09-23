
#include <algorithm>

#include "Logger.h"

#include "Image.h"
#include "ImageArray.h"
#include "ImageIO.h"

#include "ImageManager.h"


namespace gk {

ImageManager::ImageManager( ) : m_images() {}

ImageManager::~ImageManager( )
{
    for(unsigned int i= 0; i < m_images.size(); i++)
        delete m_images[i];
}
    
int ImageManager::searchPath( const std::string& path )
{
    if(std::find(m_paths.begin(), m_paths.end(), path) == m_paths.end())
        m_paths.push_back(path);
    return 0;
}

std::string ImageManager::search( const std::string& filename )
{
    return IOFileSystem::findFile(filename, m_paths);
}

Image *ImageManager::find( const std::string& filename )
{
    for(unsigned int i= 0; i < m_images.size(); i++)
        if(m_images[i]->resource.filename == filename)
            return m_images[i];
    
    return NULL;
}

Image *ImageManager::insert( Image *object )
{
    m_images.push_back( object );
    return object;
}

Image *ImageManager::read( const std::string& filename )
{
    std::string fullname= search(filename);
    Image *object= find(fullname);
    if(object != NULL)
        return object;
    
    object= ImageIO::readImage(fullname);
    if(object == NULL)
        return NULL;
    
    return insert(object);
}

Image *ImageManager::defaultImage( )
{
    return read("debug.png");
}


int ImageManager::reload( Image *object )
{
    if(object == NULL)
        return -1;
    
    if(object->released || object->resource.modified() == 1)
    {
        object->release();
        Image *tmp= ImageIO::readImage(object->resource.filename);
        if(tmp == NULL)
            return -1;
        
        // autoriser uniquement le changement de contenu ? ou autoriser aussi le changement complet : dimension, type, etc.
        object->create(tmp->width, tmp->height, tmp->depth, tmp->channels, tmp->type, tmp->data);
        delete tmp;
    }
    
    return 0;
}

int ImageManager::reload( )
{
    for(unsigned int i= 0; i < m_images.size(); i++)
        reload(m_images[i]);
    
    return 0;
}

ImageArray *readImageArray( const std::string& format, const unsigned int size )
{
    if(format.size() == 0)
        return NULL;
    
    char tmp[1024];
    ImageArray *array= new ImageArray();
    
    for(unsigned int i= 0; i < size; i++)
    {
        sprintf(tmp, format.c_str(), (int) i+1);
        
        std::string file= searchImage(tmp);
        if(IOFileSystem::isFilename(file) == false)
            break;
        
        array->push_back( readImage(file) );
    }
    
    if(array->images.size() == 0)
        WARNING("loading %u images '%s': no image. failed.\n", size, format.c_str());
    
    return array;
}    

int writeImageArray( const std::string& format, ImageArray *array )
{
    if(format.size() == 0)
        return -1;
    if(array == NULL || array->images.size() == 0)
        return -1;
    
    char tmp[1024];
    for(unsigned int i= 0; i < array->images.size(); i++)
    {
        sprintf(tmp, format.c_str(), i+1);
        if(writeImage(tmp, array->images[i]) < 0)
            return -1;
    }
    
    return 0;
}

}       // namespace
