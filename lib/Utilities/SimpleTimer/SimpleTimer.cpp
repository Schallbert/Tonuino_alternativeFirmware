#include "SimpleTimer.h"

// counts menu timer if active
void SimpleTimer::timerTick()
{
    if (m_timeout == 0)
    {
        m_elapsed = false;
        return; // timer stopped
    }

    if (!m_elapsed)
    {
        ++m_count;
    }

    if (m_count >= m_timeout)
    {
        m_elapsed = true; // timer elapsed
    }
}

void SimpleTimer::start(uint16_t ui16Timeout)
{
    m_timeout = ui16Timeout;
    m_running = true;
}

void SimpleTimer::stop()
{
    m_timeout = 0;
    m_count = 0;
    m_elapsed = false;
    m_running = false;
}

bool SimpleTimer::isElapsed()
{
    return m_elapsed;
}

bool SimpleTimer::isRunning()
{
    return m_running;
}