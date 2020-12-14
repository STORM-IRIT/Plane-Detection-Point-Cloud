#include <PDPC/Common/File.h>

namespace pdpc {

std::string get_extension(const std::string& path)
{
    const auto dot   = path.find_last_of('.');
    const auto slash = path.find_last_of('/');

    const bool has_slash = (slash != std::string::npos);
    const bool has_dot   = (dot   != std::string::npos);

           if(!has_dot && !has_slash) return "";
    else   if(!has_dot &&  has_slash) return "";
    else   if( has_dot && !has_slash) return path.substr(dot+1);
    else// if( has_dot &&  has_slash)
    {
        if(slash < dot) return path.substr(dot+1);
        else            return "";
    }
}

std::string get_filename(const std::string& path)
{
    const auto slash = path.find_last_of('/');
    const auto dot   = path.find_last_of('.');

    const bool has_slash = (slash != std::string::npos);
    const bool has_dot   = (dot   != std::string::npos);

           if(!has_dot && !has_slash) return path;
    else   if(!has_dot &&  has_slash) return path.substr(slash+1);
    else   if( has_dot && !has_slash) return path.substr(0, dot);
    else// if( has_dot &&  has_slash)
    {
        if(dot < slash) return path.substr(slash+1);
        else            return path.substr(slash+1, dot-slash-1);
    }
}

} // namespace pdpc
