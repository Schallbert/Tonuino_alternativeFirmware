#include "KeepAlive.h"

//Interface implementation for active low TTL/Relay logic
KeepAlive::KeepAlive(Arduino_interface_pins *pPinCtrl,
                     uint8_t ui8PinID,
                     bool bPinActiveState) : m_pPinCtrl(pPinCtrl),
                                             m_ui8PinID(ui8PinID),
                                             m_bPinActiveState(bPinActiveState)
{
    // Keep alive command right at constructor call
    m_pPinCtrl->pin_mode(m_ui8PinID, OUTPUT);
    m_pPinCtrl->digital_write(m_ui8PinID, m_bPinActiveState);
}

void KeepAlive::keep_alive()
{
    // Enables power supply circuit keep alive
    m_pPinCtrl->digital_write(m_ui8PinID, m_bPinActiveState);
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
        m_pPinCtrl->digital_write(m_ui8PinID, !m_bPinActiveState);
    }
}