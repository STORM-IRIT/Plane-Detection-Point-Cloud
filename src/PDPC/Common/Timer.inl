#include <PDPC/Common/Timer.h>

namespace pdpc {

// Timing ----------------------------------------------------------------------

void Timer::start()
{
    m_start = Clock::now();
}

void Timer::restart()
{
    m_start = Clock::now();
}

Timer::ldouble Timer::time_sec() const
{
    return ldouble(time_milli_sec())*1e-3;
}

Timer::ullint Timer::time_milli_sec() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(time()).count();
}

Timer::Duration Timer::time() const
{
    return Clock::now()-m_start;
}

} // namespace pdpc
