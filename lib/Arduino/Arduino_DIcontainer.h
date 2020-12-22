#ifndef ARDUINO_DICONTAINER_H
#define ARDUINO_DICONTAINER_H

#include "Arduino_DIcontainer_interface.h"
#include "Arduino_interface.h"

#include "Arduino_implementation.h"

class Arduino_DIcontainer : public Arduino_DIcontainer_interface
{
public:
    Arduino_DIcontainer() = default;
    ~Arduino_DIcontainer() = default;

public:
    // public methods returning a pointer to the implementation via public interface
    Arduino_interface_pins &getPins() override;
    Arduino_interface_delay &getDelay() override;
    Arduino_interface_com &getSerial() override;
    Arduino_interface_random &getRandom() override;
    Arduino_interface_eeprom &getEeprom() override;

private:
    Arduino_pins m_Pins{Arduino_pins()};
    Arduino_delay m_Delay{Arduino_delay()};
    Arduino_com m_Serial{Arduino_com()};
    Arduino_random m_Random{Arduino_random()};
    Arduino_eeprom m_Eeprom{Arduino_eeprom()};
};

#endif // ARDUINO_DICONTAINER_H