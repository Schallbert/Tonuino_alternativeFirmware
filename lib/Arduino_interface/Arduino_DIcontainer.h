#ifndef ARDUINO_DICONTAINER_H
#define ARDUINO_DICONTAINER_H

#include "Arduino_DIcontainer_interface.h"
#include "Arduino_implementation.h"

class Arduino_DIcontainer : public Arduino_DIcontainer_interface
{
public:
    Arduino_DIcontainer()
    {
        m_pPins = new Arduino_pins();
        m_pDelay = new Arduino_delay();
        m_pSerial = new Arduino_com();
        m_pRandom = new Arduino_random();
        m_pEeprom = new Arduino_eeprom();
    };

    ~Arduino_DIcontainer(){
        delete m_pPins;
        delete m_pDelay;
        delete m_pSerial;
        delete m_pRandom;
        delete m_pEeprom;
    };

public:
    Arduino_interface_pins *getPins() override { return m_pPins; };
    Arduino_interface_delay *getDelay() override { return m_pDelay; };
    Arduino_interface_com *getSerial() override { return m_pSerial; };
    Arduino_interface_random *getRandom() override { return m_pRandom; };
    Arduino_interface_eeprom *getEeprom() override { return m_pEeprom; };

private:
    Arduino_interface_pins *m_pPins{nullptr};
    Arduino_interface_delay *m_pDelay{nullptr};
    Arduino_interface_com *m_pSerial{nullptr};
    Arduino_interface_random *m_pRandom{nullptr};
    Arduino_interface_eeprom *m_pEeprom{nullptr};
};

#endif // ARDUINO_DICONTAINER_H