#include <PDPC/Common/Progress.h>
#include <PDPC/Common/Log.h>
#include <PDPC/Common/Timer.h>

#include <cmath>

namespace pdpc {

Progress::Progress(int step_count, int percentage_increment, bool enable) :
    m_step(0),
    m_step_count(step_count),
    m_step_increment(std::ceil(double(step_count)/(100.0/percentage_increment))),
    m_percentage_increment(percentage_increment),
    m_next_step(m_step_increment),
    m_next_percentage(percentage_increment),
    m_timer(std::make_unique<Timer>()),
    m_enabled(enable)
{
}

Progress::Progress() :
    Progress(0, 0, false)
{
}

Progress::Progress(int step_count) :
    Progress(step_count, 10, true)
{
}

Progress::Progress(int step_count, int percentage_increment) :
    Progress(step_count, percentage_increment, true)
{
}

Progress::Progress(int step_count, bool enable) :
    Progress(step_count, 10, enable)
{
}

Progress::Progress(const Progress &other) :
    m_step(other.m_step),
    m_step_count(other.m_step_count),
    m_step_increment(other.m_step_increment),
    m_percentage_increment(other.m_percentage_increment),
    m_next_step(other.m_next_step),
    m_next_percentage(other.m_next_percentage),
    m_timer(std::make_unique<Timer>(*other.m_timer)),
    m_enabled(other.m_enabled)
{
}

Progress& Progress::operator =(const Progress& other)
{
    m_step = other.m_step;
    m_step_count = other.m_step_count;
    m_step_increment = other.m_step_increment;
    m_percentage_increment = other.m_percentage_increment;
    m_next_step = other.m_next_step;
    m_next_percentage = other.m_next_percentage;
    *m_timer = *other.m_timer;
    m_enabled = other.m_enabled;
    return *this;
}

Progress::~Progress()
{
}

void Progress::operator ++()
{
    this->incr("");
}

void Progress::incr(const char* prefix)
{
    if(m_enabled)
    {
        #pragma omp critical (Progress_update)
        {
            ++m_step;
            if(m_step == m_step_count)
            {
                long double total_time = m_timer->time_milli_sec();
                info() << prefix << m_step << "/" << m_step_count
                       << " [" << m_next_percentage << "%]"
                       << " total time = " << Timer::millisec_str(total_time);
            }
            else if(m_step >= m_next_step)
            {
                long double elapsed_time = m_timer->time_milli_sec();
                long double remaining_time = elapsed_time * (100. - m_next_percentage) / m_next_percentage;

                info() << prefix << m_step << "/" << m_step_count
                       << " [" << m_next_percentage <<"%]"
                       << " remaining time = " << Timer::millisec_str(remaining_time);

                // increment current state
                m_next_step       += m_step_increment;
                m_next_percentage += m_percentage_increment;
            }
        }
    }
}

} // namespace pdpc
