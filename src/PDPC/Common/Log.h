#pragma once

#include <PDPC/Common/Timer.h>

#include <iostream>
#include <string>
#include <sstream>

namespace pdpc {

class LogMessage;

enum LogLevel : int
{
    Critical  =  0,
    Error,    // 1
    Warning,  // 2
    Info,     // 3
    Debug     // 4
//  Debug1    // 5 (Debug+1)
//  Debug2    // 6 (Debug+2)
//  ...
};

inline LogMessage log(int level, const std::string& context = "");
inline LogMessage log(const std::string& context = "", int level = Info);

inline LogMessage critical(const std::string& context = "");
inline LogMessage error(const std::string& context = "");
inline LogMessage warning(const std::string& context = "");
inline LogMessage info(const std::string& context = "");

inline LogMessage debug(const std::string& context = "", int debug_level = 0);
inline LogMessage debug(int debug_level, const std::string& context = "");

class LogMessage
{
public:
    inline LogMessage(int level, const std::string& context);
    inline LogMessage(const LogMessage& other);
    inline ~LogMessage();

    template<typename T>
    inline LogMessage& operator << (const T& val);
    template<typename T>
    inline LogMessage& operator << (T& val);

    inline LogMessage& iff(bool b);

protected:
    int m_level;
    bool m_on;
    std::ostringstream m_os;
};

} // namespace pdpc

#include <PDPC/Common/Log.inl>
