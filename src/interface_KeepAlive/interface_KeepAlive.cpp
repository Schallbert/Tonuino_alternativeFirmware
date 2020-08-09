#include "interface_KeepAlive.h"

//Interface implementation for active low TTL/Relay logic
KeepAlive::KeepAlive(uint8_t ui8PinID,
                     bool bPinActiveState,
                     uint16_t ui16Seconds) : m_ui8PinID(ui8PinID),
                                             m_bPinActiveState(bPinActiveState),
                                             m_ui16Seconds(ui16Seconds)
{
    // Keep alive command right at constructor call
    pinMode(m_ui8PinID, OUTPUT);
    digitalWrite(m_ui8PinID, m_bPinActiveState);
}

void KeepAlive::keep_alive()
{
    digitalWrite(m_ui8PinID, m_bPinActiveState);
}

void KeepAlive::request_shutdown()
{
    m_bShutDownRequested = true;
}

bool KeepAlive::get_shutdown_request()
{
    return m_bShutDownRequested;
}

void KeepAlive::allow_shutdown()
{
    if (m_bShutDownRequested)
    {
        digitalWrite(m_ui8PinID, !m_bPinActiveState);
    }
}

void KeepAlive::set_idle_timer(bool bIdleTimerActive)
{
    m_bIdleTimerActive = bIdleTimerActive;
}

void KeepAlive::idle_timer_tick1ms()
{
    if (m_bIdleTimerActive)
    {
        ++m_ui32Tick1ms;
        if (m_ui32Tick1ms / 1000 >= m_ui16Seconds)
        {
            m_ui32Tick1ms = 0;
            m_bIdleTimerActive = false;
            m_bShutDownRequested = true;
        }
    }
    else
    {
        m_ui32Tick1ms = 0;
    }
}