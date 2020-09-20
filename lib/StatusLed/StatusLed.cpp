#include "StatusLed.h"
      
StatusLed::StatusLed(uint8_t ledPin, uint16_t flashSlowMillis, uint16_t flashQuickMillis, bool active)
{
    this->ledPin = ledPin;
    this->flashSlowMillis = flashSlowMillis;
    this->flashQuickMillis = flashQuickMillis;
    this->active = active;
    pinMode(ledPin, OUTPUT);
    this->perform = &StatusLed::led_off; //Set default behavior of function pointer: LED off
    this->msCount = 0;
    digitalWrite(ledPin, !active); //init state is off
}

void StatusLed::led_service()
{
    msCount++;
    (this->*perform)(); // Call function that pointer points to.
    // this-> is needed to bind to current instance's member function
}

void StatusLed::set_led_behavior(eLedState ledState)
{
    // Set function pointer. 
    // This is to keep execution times in interrupt routine as low as possible
    ledBehaviorSet = true;
    switch (ledState)
    {
        case solid :
            perform = &StatusLed::led_solid;
            break;
        case flash_slow :
            perform = &StatusLed::led_flash_slow;
            break;
        case flash_quick :
            perform = &StatusLed::led_flash_quick;
            break;
        case dim :
            perform = &StatusLed::led_dim;
        default:
            perform = &StatusLed::led_off;
            break;
    }
}

void StatusLed::led_off()
{
    digitalWrite(ledPin, !active);
}

void StatusLed::led_solid()
{
    digitalWrite(ledPin, active);
}

void StatusLed::led_flash_slow()
{
    if (msCount >= flashSlowMillis)
    {
        msCount = 0;
        digitalWrite(ledPin, !digitalRead(ledPin));
    }
}

void StatusLed::led_flash_quick()
{
    if (msCount >= flashQuickMillis)
    {
        msCount = 0;
        digitalWrite(ledPin, !digitalRead(ledPin));
    }
}

void StatusLed::led_dim()
{
    bool dim = false;
    if(active)
    {
        dim = msCount & 0x08;
    }
    else
    {
        dim = !(msCount & 0x08);
    }
    
    digitalWrite(ledPin, dim); // Sets LED to 12.5% brightness @ 125 Hz
}