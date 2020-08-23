#include "SimpleTimer.h"


/*
TimerManager::TimerManager(uint8_t ui8MaxTimersToKeep)
{
    // Initialize array of timers to register
    // pointer to an array of pointers. Crazy.
    m_pArrayOfTimers = new SimpleTimer *[ui8MaxTimersToKeep] {};
}

TimerManager::~TimerManager()
{
    delete m_pArrayOfTimers;
}

void TimerManager::register_timer(SimpleTimer *pTimer)
{
    m_pArrayOfTimers[m_ui8NumOfElements] = pTimer;
    ++m_ui8NumOfElements;
}

void TimerManager::timer_tick()
{
    // execute timer_tick for all registered timers
    for (uint8_t i = 0; i < m_ui8NumOfElements; ++i)
    {
        m_pArrayOfTimers[i]->timer_tick();
    }
}
*/


// counts menu timer if active
void SimpleTimer::timer_tick()
{
    if (m_ui16Timeout == 0)
    {
        m_bElapsed = false;
        return; // timer stopped
    }

    if (m_ui16Count < m_ui16Timeout)
    {
        ++m_ui16Count; // timer running
    }
    else
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