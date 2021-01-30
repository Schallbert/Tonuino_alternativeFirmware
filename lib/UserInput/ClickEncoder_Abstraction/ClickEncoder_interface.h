#ifndef CLICKENCODER_INTERFACE_H
#define CLICKENCODER_INTERFACE_H

#include "../Arduino_HardwareAbstraction/Arduino_types.h"

class ClickEncoder_interface
{
public:
    enum eButtonState
    {
        Open = 0,
        Closed,
        Pressed,
        Held,
        Released,
        Clicked,
        DoubleClicked,
        LongPressRepeat
    };

public:
    virtual ~ClickEncoder_interface(){};

public:
    // Routine to internally poll encoder/button status
    virtual void service() = 0;
    // retrieve encoder value from hardware
    virtual int16_t getValue() = 0;
    // retrieve button status from hardware
    virtual eButtonState getButton() = 0;
    // acceleration feature of encoder: Quick turn will over-proportionally bump values
    virtual void setAccelerationEnabled(const bool &enabled) = 0;
    // doubleclick feature of encoder button.
    virtual void setDoubleClickEnabled(const bool &enabled) = 0;
};

#endif //CLICKENCODER_INTERFACE_H