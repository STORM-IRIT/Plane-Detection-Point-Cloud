#include <PDPC/Common/Log.h>

namespace pdpc {

LogMessage log(int level, const std::string& context)        {return LogMessage(level, context);}
LogMessage log(const std::string& context, int level)        {return LogMessage(level, context);}

LogMessage critical(const std::string& context)              {return LogMessage(Critical, context);}
LogMessage error(const std::string& context)                 {return LogMessage(Error,    context);}
LogMessage warning(const std::string& context)               {return LogMessage(Warning,  context);}
LogMessage info(const std::string& context)                  {return LogMessage(Info,     context);}

LogMessage debug(const std::string& context, int debug_level){return LogMessage(Debug + debug_level, context);}
LogMessage debug(int debug_level, const std::string& context){return LogMessage(Debug + debug_level, context);}

LogMessage::LogMessage(int level, const std::string& context) :
    m_level(level),
    m_on(true),
    m_os()
{
    m_os << "PDPC - " << Timer::date() << " - ";

         if(m_level == Critical) m_os << "[critical] ";
    else if(m_level == Error)    m_os << "[error] ";
    else if(m_level == Warning)  m_os << "[warning] ";
    else if(m_level >= Debug)    m_os << "[debug] ";

    if(!context.empty())
    {
        m_os << context << ": ";
    }
}

LogMessage::LogMessage(const LogMessage& other) :
    m_level(other.m_level),
    m_on(other.m_on),
    m_os()
{
    m_os << other.m_os.str();
}

LogMessage::~LogMessage()
{
    if(m_on)
    {
        if(m_level < Info)
            std::cerr << m_os.str() << std::endl;
        else
            std::cout << m_os.str() << std::endl;
    }
}

template<typename T>
LogMessage& LogMessage::operator << (const T& val)
{
    m_os << val;
    return *this;
}

template<typename T>
LogMessage& LogMessage::operator << (T& val)
{
    m_os << val;
    return *this;
}

LogMessage& LogMessage::iff(bool b)
{
    m_on = b;
    return *this;
}

} // namespace pdpc
