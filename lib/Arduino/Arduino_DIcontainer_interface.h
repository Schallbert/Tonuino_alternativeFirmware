#ifndef ARDUINO_DICONTAINER_INTERFACE_H
#define ARDUINO_DICONTAINER_INTERFACE_H

#include "../Arduino/Arduino_interface.h"

class Arduino_DIcontainer_interface
{
public:
    virtual ~Arduino_DIcontainer_interface(){};

public:
    virtual Arduino_interface_pins *getPins() = 0;
    virtual Arduino_interface_delay *getDelay() = 0;
    virtual Arduino_interface_com *getSerial() = 0;
    virtual Arduino_interface_random *getRandom() = 0;
    virtual Arduino_interface_eeprom *getEeprom() = 0;
};

#endif // ARDUINO_DICONTAINER_INTERFACE_H