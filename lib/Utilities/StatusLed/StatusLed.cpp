#include "StatusLed.h"

StatusLed::StatusLed(Arduino_interface_pins *pPins,
                     uint8_t ledPinId,
                     bool pinActiveState,
                     uint16_t msFlashSlow,
                     uint16_t msFlashQuick) : m_pPinControl(pPins),
                                              m_ui8LedPinId(ledPinId),
                                              m_bPinAciveState(pinActiveState),
                                              m_ui16MsFlashSlow(msFlashSlow),
                                              m_ui16MsFlashQuick(msFlashQuick)
{
    m_pPinControl->pin_mode(m_ui8LedPinId, OUTPUT);
    this->perform = &StatusLed::led_off; //Set default behavior of function pointer: LED off
    this->m_ui16TickInternal = 0;
    m_pPinControl->digital_write(m_ui8LedPinId, !m_bPinAciveState); //init state is off
}

void StatusLed::led_service()
{
    m_ui16TickInternal++;
    (this->*perform)(); // Call function that pointer points to.
    // this-> is needed to bind to current instance's member function
}

void StatusLed::set_led_behavior(eLedState ledState)
{
    // Set function pointer.
    // This is to keep execution times in interrupt routine as low as possible
    m_bLedBehaviorSet = true;
    switch (ledState)
    {
    case solid:
        perform = &StatusLed::led_solid;
        break;
    case flash_slow:
        perform = &StatusLed::led_flash_slow;
        break;
    case flash_quick:
        perform = &StatusLed::led_flash_quick;
        break;
    case dim:
        perform = &StatusLed::led_dim;
    default:
        perform = &StatusLed::led_off;
        break;
    }
}

void StatusLed::led_off()
{
    m_pPinControl->digital_write(m_ui8LedPinId, !m_bPinAciveState);
}

void StatusLed::led_solid()
{
    m_pPinControl->digital_write(m_ui8LedPinId, m_bPinAciveState);
}

void StatusLed::led_flash_slow()
{
    if (m_ui16TickInternal >= m_ui16MsFlashSlow)
    {
        m_ui16TickInternal = 0;
        m_pPinControl->digital_write(m_ui8LedPinId, !m_pPinControl->digital_read(m_ui8LedPinId));
    }
}

void StatusLed::led_flash_quick()
{
    if (m_ui16TickInternal >= m_ui16MsFlashQuick)
    {
        m_ui16TickInternal = 0;
        m_pPinControl->digital_write(m_ui8LedPinId, m_pPinControl->digital_read(m_ui8LedPinId));
    }
}

void StatusLed::led_dim()
{
    bool dim = false;
    if (m_bPinAciveState)
    {
        dim = m_ui16TickInternal & 0x08;
    }
    else
    {
        dim = !(m_ui16TickInternal & 0x08);
    }

    m_pPinControl->digital_write(m_ui8LedPinId, dim); // Sets LED to 12.5% brightness @ 125 Hz
}