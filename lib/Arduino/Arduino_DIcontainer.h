#ifndef ARDUINO_DICONTAINER_H
#define ARDUINO_DICONTAINER_H

#include "Arduino_DIcontainer_interface.h"
#include "Arduino_interface.h"

class Arduino_DIcontainer : public Arduino_DIcontainer_interface
{
public:
    Arduino_DIcontainer();
    ~Arduino_DIcontainer();

public:
    // public methods returning a pointer to the implementation via public interface
    Arduino_interface_pins *getPins() override;
    Arduino_interface_delay *getDelay() override;
    Arduino_interface_com *getSerial() override;
    Arduino_interface_random *getRandom() override;
    Arduino_interface_eeprom *getEeprom() override;

private:
    Arduino_interface_pins *m_pPins{nullptr};
    Arduino_interface_delay *m_pDelay{nullptr};
    Arduino_interface_com *m_pSerial{nullptr};
    Arduino_interface_random *m_pRandom{nullptr};
    Arduino_interface_eeprom *m_pEeprom{nullptr};
};

#endif // ARDUINO_DICONTAINER_H