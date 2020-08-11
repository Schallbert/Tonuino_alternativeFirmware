#include "interface_KeepAlive.h"

//Interface implementation for active low TTL/Relay logic
KeepAlive::KeepAlive(uint8_t ui8PinID,
                     bool bPinActiveState) : m_ui8PinID(ui8PinID),
                                             m_bPinActiveState(bPinActiveState)
{
    // Keep alive command right at constructor call
    pinMode(m_ui8PinID, OUTPUT);
    digitalWrite(m_ui8PinID, m_bPinActiveState);
}

void KeepAlive::keep_alive()
{
    // Enables power supply circuit keep alive
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
    // disables power supply circuit
    if (m_bShutDownRequested)
    {
        digitalWrite(m_ui8PinID, !m_bPinActiveState); 
    }
}