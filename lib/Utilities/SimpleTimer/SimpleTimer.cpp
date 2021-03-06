#include "SimpleTimer.h"

// counts menu timer if active
void SimpleTimer::timerTick()
{
    if (m_timeout == 0)
    {
        return; // timer stopped
    }

    if (m_count < m_timeout)
    {
        ++m_count; // timer running
    }
}

void SimpleTimer::start(uint16_t ui16Timeout)
{
    m_timeout = ui16Timeout;
}

void SimpleTimer::stop()
{
    m_timeout = 0;
    m_count = 0;
}

bool SimpleTimer::isElapsed()
{
    return (m_timeout && (m_count >= m_timeout));
}

bool SimpleTimer::isRunning()
{
    return (m_count < m_timeout);
}