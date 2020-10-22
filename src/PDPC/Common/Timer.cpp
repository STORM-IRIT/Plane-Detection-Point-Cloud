#include <PDPC/Common/Timer.h>

#include <cmath>
#include <iostream>
#include <algorithm>

namespace pdpc {

// Timer -----------------------------------------------------------------------

Timer::Timer(bool start)
{
    if(start)
    {
        this->start();
    }
}

// String ----------------------------------------------------------------------

Timer::String Timer::smart_str()
{
    return millisec_str(this->time_milli_sec());
}

Timer::String Timer::millisec_str(ldouble millisec)
{
    int day = int(millisec/86400000);
    if(day > 0)
    {
        millisec -= day*86400000;
        int hour  = int(millisec/3600000);
        // print day + hour
        return std::to_string(day) + "d " + std::to_string(hour) + "h";
    }

    int hour = int(millisec/3600000);
    if(hour > 0)
    {
        millisec -= hour*3600000;
        int min   = int(millisec/60000);
        // print hour + min
        return std::to_string(hour) + "h " + std::to_string(min) + "min";
    }

    int min = int(millisec/60000);
    if(min > 0)
    {
        millisec -= min*60000;
        int sec   = int(millisec/1000);
        // print min + sec
        return std::to_string(min) + "min " + std::to_string(sec) + "s";
    }

    int sec = int(millisec/1000);
    if(sec > 0)
    {
        millisec -= sec*1000;
        // print sec + milli
        return std::to_string(sec) + "s " + std::to_string(int(millisec)) + "ms";
    }

    // print milli
    return std::to_string(int(millisec)) + "ms";
}

Timer::String Timer::sec_str(ldouble sec)
{
    int day = int(sec/86400);
    if(day > 0)
    {
        sec -= day*86400;
        int hour  = int(sec/3600);
        // print day + hour
        return std::to_string(day) + "d " + std::to_string(hour) + "h";
    }

    int hour = int(sec/3600);
    if(hour > 0)
    {
        sec -= hour*3600;
        int min   = int(sec/60);
        // print hour + min
        return std::to_string(hour) + "h " + std::to_string(min) + "min";
    }

    int min = int(sec/60);
    if(min > 0)
    {
        sec -= min*60;
        // print min + sec
        return std::to_string(min) + "min " + std::to_string(int(sec)) + "s";
    }

    // print sec
    return std::to_string(int(sec)) + "s";
}

// Date ------------------------------------------------------------------------

Timer::String Timer::date()
{
    time_t t = SystemClock::to_time_t(SystemClock::now());
    char buffer[64];

    // ex: 2019-03-28 16:30:25
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&t));

    return buffer;
}

Timer::String Timer::date_str()
{
    time_t t = SystemClock::to_time_t(SystemClock::now());
    char buffer[64];

    // ex: 2019-03-28_16-30-25
    strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", localtime(&t));

    return buffer;
}

} // namespace pdpc
