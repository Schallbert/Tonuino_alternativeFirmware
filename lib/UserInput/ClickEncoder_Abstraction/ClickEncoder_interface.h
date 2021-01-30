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
        LongPressRepeat,
        Released,
        Clicked,
        DoubleClicked
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
    // enhanced feature control
    virtual void setAccelerationEnabled(const bool &enabled) = 0;
    virtual void setLongPressRepeatEnabled(const bool &enabled) = 0;
    virtual void setDoubleClickEnabled(const bool &enabled) = 0;
};

#endif //CLICKENCODER_INTERFACE_H