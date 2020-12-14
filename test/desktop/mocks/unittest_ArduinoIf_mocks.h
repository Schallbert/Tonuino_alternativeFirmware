#ifndef ARDUINOIF_MOCKS_H
#define ARDUINOIF_MOCKS_H

#include <gmock/gmock.h>
#include "../Arduino_HardwareAbstraction/Arduino_types.h"
#include "../Arduino_HardwareAbstraction/Arduino_interface.h"

// Interface class for Arduino pin manipulation functions
// Preparation for mocking
class Mock_pinCtrl : public Arduino_interface_pins
{
public:
    MOCK_METHOD(void, pin_mode, (uint8_t pinId, uint8_t mode), (override));
    MOCK_METHOD(void, digital_write, (uint8_t pinId, uint8_t output), (override));
    MOCK_METHOD(int, digital_read, (uint8_t pinId), (override));
    MOCK_METHOD(int, analog_read, (uint8_t pinId), (override));
    MOCK_METHOD(void, analog_reference, (uint8_t mode), (override));
    MOCK_METHOD(void, analog_write, (uint8_t pinId, int output), (override));
};

// Class to interface Arduino's delay and elapsed time functions
class Mock_delay : public Arduino_interface_delay
{
public:
    MOCK_METHOD(unsigned long, milli_seconds, (), (override));
    MOCK_METHOD(unsigned long, micro_seconds, (), (override));
    MOCK_METHOD(void, delay_ms, (unsigned long ms), (override));
    MOCK_METHOD(void, delay_us, (unsigned int us), (override));
};

// Class to interface Arduino's com port and print out information
// All info taken from "USBAPI.h" & "CDC.cpp"
class Mock_serial : public Arduino_interface_com
{
public:
    MOCK_METHOD(void, com_begin, (unsigned long baudrate), (override));
    MOCK_METHOD(void, com_print, (uint8_t i), (override));
    MOCK_METHOD(void, com_println, (const char *str), (override));
};

class Mock_random : public Arduino_interface_random
{
public:
    MOCK_METHOD(void, random_generateSeed, (byte floatingAnalogPin_Id), (override));
    MOCK_METHOD(uint8_t, random_generateUi8, (), (override));
};

class Mock_eeprom : public Arduino_interface_eeprom
{
public:
    MOCK_METHOD(uint8_t, eeprom_read, (uint8_t memId), (override));
    MOCK_METHOD(void, eeprom_write, (uint8_t memId, uint8_t contents), (override));
};
#endif //ARDUINO_INTERFACE_H#endif // ARDUINOIF_MOCKS_H