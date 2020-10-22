#pragma once

#include <memory>

namespace pdpc {

class Timer;

class Progress
{
public:
    Progress();
    Progress(int step_count);
    Progress(int step_count, int percentage_increment);
    Progress(int step_count, int percentage_increment, bool enable);
    Progress(int step_count, bool enable);

    Progress(const Progress& other);
    Progress& operator =(const Progress& other);

    ~Progress();

public:
    void operator ++();
    void incr(const char* prefix);

protected:
    int m_step;
    int m_step_count;

    int m_step_increment;
    int m_percentage_increment;

    int m_next_step;
    int m_next_percentage;

    std::unique_ptr<Timer> m_timer;
    bool m_enabled;
};

} // namespace pdpc
