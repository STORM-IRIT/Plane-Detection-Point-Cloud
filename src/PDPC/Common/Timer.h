#pragma once

#include <time.h>
#include <chrono>
#include <string>

namespace pdpc {

class Timer
{
public:
    // Types -------------------------------------------------------------------
public:
    using Clock        = std::chrono::high_resolution_clock;
    using TimePoint    = std::chrono::high_resolution_clock::time_point;
    using Duration     = std::chrono::high_resolution_clock::duration;
    using SystemClock  = std::chrono::system_clock;
    using ullint       = unsigned long long int;
    using ldouble      = long double;
    using String       = std::string;

    // Timer -------------------------------------------------------------------
public:
    Timer(bool start = true);

    // Timing ------------------------------------------------------------------
public:
    inline void start();
    inline void restart();

    inline ldouble time_sec() const;
    inline ullint  time_milli_sec() const;

private:
    inline Duration time() const;

    // String ------------------------------------------------------------------
public:
    String smart_str();

    static String millisec_str(ldouble millisec);
    static String sec_str(ldouble sec);

    // Date --------------------------------------------------------------------
public:
    static String date();
    static String date_str();

    // Data --------------------------------------------------------------------
private:
    TimePoint m_start;

}; // class Timer

} // namespace pdpc

#include <PDPC/Common/Timer.inl>
