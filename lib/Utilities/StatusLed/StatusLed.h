#ifndef STATUSLED_H
#define STATUSLED_H

class Arduino_interface_pins;

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
    StatusLed(Arduino_interface_pins &rPins,
              uint8_t ledPinId,
              bool pinActiveState = true,
              uint16_t msFlashSlow = 500,
              uint16_t msFlashQuick = 100);
    ~StatusLed() = default;
    StatusLed(const StatusLed &cpy) = delete;

public:
    // Methods
    void led_service();                        // ISR routine, to be hung to a 1ms timer interrupt. Controls LED.
    void set_led_behavior(eLedState ledState); // Defines mode in which LED should illuminate

private:
    // Methods
    void led_off();
    void led_solid();
    void led_flash_slow();  // wrapper method for led_flash()
    void led_flash_quick(); // wrapper method for led_flash()
    void led_flash(uint16_t msFlashInterval);
    void led_dim();

private:
    Arduino_interface_pins &m_rPinControl;
    const uint8_t m_ui8LedPinId;
    const bool m_bPinAciveState;
    const uint16_t m_ui16MsFlashSlow;
    const uint16_t m_ui16MsFlashQuick;

    void (StatusLed::*perform)(); //class method pointer
    volatile uint16_t m_ui16TickInternal{0};
};
#endif // STATUSLED_H