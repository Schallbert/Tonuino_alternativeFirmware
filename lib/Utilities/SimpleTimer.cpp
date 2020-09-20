#include "SimpleTimer.h"

// counts menu timer if active
void SimpleTimer::timer_tick()
{
    if (m_ui16Timeout == 0)
    {
        m_bElapsed = false;
        return; // timer stopped
    }

    if(!m_bElapsed)
    {
        ++m_ui16Count;
    }

    if (m_ui16Count >= m_ui16Timeout)
    {
        m_bElapsed = true; // timer elapsed
    }
}

void SimpleTimer::start(uint16_t ui16Timeout)
{
    m_ui16Timeout = ui16Timeout;
}

void SimpleTimer::stop()
{
    m_ui16Timeout = 0;
    m_ui16Count = 0;
    m_bElapsed = false;
}

bool SimpleTimer::is_elapsed()
{
    return m_bElapsed;
}