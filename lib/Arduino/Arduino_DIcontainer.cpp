#include "Arduino_DIcontainer.h"

Arduino_interface_pins &Arduino_DIcontainer::getPins()
{
    return m_Pins;
}

Arduino_interface_delay &Arduino_DIcontainer::getDelay()
{
    return m_Delay;
}

Arduino_interface_com &Arduino_DIcontainer::getSerial()
{
    return m_Serial;
}

Arduino_interface_random &Arduino_DIcontainer::getRandom()
{
    return m_Random;
}

Arduino_interface_eeprom &Arduino_DIcontainer::getEeprom()
{
    return m_Eeprom;
}