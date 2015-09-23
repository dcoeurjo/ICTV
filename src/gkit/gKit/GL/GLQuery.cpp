
#include "GL/GLQuery.h"

namespace gk {
    
    
std::string GLCounterState::summary( const std::string& message ) 
{
    char tmp[1024];
    sprintf(tmp, "%s: cpu %6lu [%6lu -- %6lu]  gpu %6lu [%6lu -- %6lu]", message.c_str(), 
        cpu_time, cpu_start, cpu_stop, 
        gpu_time, gpu_start, gpu_stop);
    return std::string(tmp);
}    

std::string OverlapGPUSummary( const std::string &message, const GLCounterState& a, const GLCounterState& b )
{
    char tmp[1024];
    if(a.gpu_start < b.gpu_stop && a.gpu_stop > b.gpu_start)
        sprintf(tmp, "%s:    overlap [%6lu -- %6lu]", message.c_str(), std::max(a.gpu_start, b.gpu_start), std::min(a.gpu_stop, b.gpu_stop));
    else
        sprintf(tmp, "%s: no overlap [%6lu -- %6lu]", message.c_str(), std::max(a.gpu_start, b.gpu_start), std::min(a.gpu_stop, b.gpu_stop));
    
    return std::string(tmp);
}

std::string OverlapCPUSummary( const std::string &message, const GLCounterState& a, const GLCounterState& b )
{
    char tmp[1024];
    if(a.cpu_start < b.cpu_stop && a.cpu_stop > b.cpu_start)
        sprintf(tmp, "%s:    overlap [%6lu -- %6lu]", message.c_str(), std::max(a.cpu_start, b.cpu_start), std::min(a.cpu_stop, b.cpu_stop));
    else
        sprintf(tmp, "%s: no overlap [%6lu -- %6lu]", message.c_str(), std::max(a.cpu_start, b.cpu_start), std::min(a.cpu_stop, b.cpu_stop));
    
    return std::string(tmp);
}

std::string GLQuery::summary( const std::string& message ) 
{
    GLint64 cpu_value= cpu_stop - cpu_start;
    char tmp[1024];
    sprintf(tmp, "%s: cpu % 6ldus / gpu % 3ldms % 3ldus", message.c_str(), cpu_value / 1000, value / 1000000, (value / 1000) % 1000);
    return std::string(tmp);
}

std::string GLCounter::summary( const std::string& message ) 
{
    char tmp[1024];
    sprintf(tmp, "%s: cpu % 6ldus / gpu % 3ldms % 3ldus", message.c_str(), cpu_value / 1000, value / 1000000, (value / 1000) % 1000);
    return std::string(tmp);
}

}       // namespace
