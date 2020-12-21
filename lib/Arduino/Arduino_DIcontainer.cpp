#include "Arduino_implementation.h"
#include "Tonuino_config.h"

#include "Arduino_DIcontainer.h"

Arduino_DIcontainer::Arduino_DIcontainer()
{
    // implementations
    m_pPins = new Arduino_pins();
    m_pDelay = new Arduino_delay();
    m_pSerial = new Arduino_com();
    m_pRandom = new Arduino_random();
    m_pEeprom = new Arduino_eeprom();

    m_pSerial->com_begin(DEBUGSERIAL_BAUDRATE);
}

Arduino_DIcontainer::~Arduino_DIcontainer()
{
    delete m_pPins;
    delete m_pDelay;
    delete m_pSerial;
    delete m_pRandom;
    delete m_pEeprom;
}

Arduino_interface_pins *Arduino_DIcontainer::getPins()
{
    return m_pPins;
}

Arduino_interface_delay *Arduino_DIcontainer::getDelay()
{
    return m_pDelay;
}

Arduino_interface_com *Arduino_DIcontainer::getSerial()
{
    return m_pSerial;
}

Arduino_interface_random *Arduino_DIcontainer::getRandom()
{
    return m_pRandom;
}

Arduino_interface_eeprom *Arduino_DIcontainer::getEeprom()
{
    return m_pEeprom;
}