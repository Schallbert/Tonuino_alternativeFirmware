#ifndef STATUSLED_H
#define STATUSLED_H

#include "Arduino.h"

class StatusLed
{
public:
    enum eLedState
    {
        off = 0,         // Off
        solid = 1,       // Solid on full brightness
        flash_slow = 2,  // flashes at a period of 2* flashSlowTime
        flash_quick = 3, // flashes at a period of 2* flashQuickTime
        dim = 4          // Solid dimmed to 12.5%
    };

public:
    // Constructor
    StatusLed(uint8_t ledPinId,
              bool pinActiveState = HIGH,
              uint16_t msFlashSlow = 500,
              uint16_t msFlashQuick = 100);

public:
    // Methods
    void led_service();                        // ISR routine, to be hung to a 1ms timer interrupt. Controls LED.
    void set_led_behavior(eLedState ledState); // Defines mode in which LED should illuminate

private:
    // Methods
    void led_off();
    void led_solid();
    void led_flash_slow();
    void led_flash_quick();
    void led_dim();

private:
    uint8_t m_ui8LedPinId;
    bool m_bPinAciveState;
    uint16_t m_ui16MsFlashSlow;
    uint16_t m_ui16MsFlashQuick;

    void (StatusLed::*perform)(); //class method pointer
    bool m_bLedBehaviorSet;
    volatile uint16_t m_ui16MsCount;
};
#endif // STATUSLED_H