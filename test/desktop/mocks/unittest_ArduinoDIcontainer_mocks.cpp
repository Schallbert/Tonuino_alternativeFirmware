#include "unittest_ArduinoDIcontainer_mocks.h"

Arduino_interface_pins *Fake_ArduinoDIcontainer::getPins() { return &m_pins; }
Arduino_interface_delay *Fake_ArduinoDIcontainer::getDelay() { return &m_delay; }
Arduino_interface_com *Fake_ArduinoDIcontainer::getSerial() { return &m_serial; }
Arduino_interface_random *Fake_ArduinoDIcontainer::getRandom() { return &m_random; }
Arduino_interface_eeprom *Fake_ArduinoDIcontainer::getEeprom() { return &m_eeprom; }
