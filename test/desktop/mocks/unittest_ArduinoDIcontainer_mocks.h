#ifndef UNITTEST_ARDUINODICONTAINER_MOCKS_H
#define UNITTEST_ARDUINODICONTAINER_MOCKS_H

class  mockArduinoDIcontainer : public Arduino_DIcontainer_interface

MOCK_METHOD(Arduino_interface_pins *, getPins, (), (override);
MOCK_METHOD(Arduino_interface_delay *, getDelay, (), (override);
MOCK_METHOD(Arduino_interface_com *, getSerial, (), (override);
MOCK_METHOD(Arduino_interface_random *, getRandom, (), (override);
MOCK_METHOD(Arduino_interface_eeprom *, getEeprom, (), (override);

#endif // UNITTEST_ARDUINODICONTAINER_MOCKS_H