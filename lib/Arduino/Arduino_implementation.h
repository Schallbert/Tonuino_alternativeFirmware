#ifndef ARDUINO_IMPLEMENTATION_H
#define ARDUINO_IMPLEMENTATION_H

#include "../Arduino_HardwareAbstraction/Arduino_interface.h"

class Arduino_pins : public Arduino_interface_pins
{
public:
    ~Arduino_pins(){};

public:
    void pin_mode(uint8_t pinId, uint8_t mode) override;
    void digital_write(uint8_t pinId, uint8_t output) override;
    int digital_read(uint8_t pinId) override;
    int analog_read(uint8_t pinId) override;
    void analog_reference(uint8_t mode) override;
    void analog_write(uint8_t pinId, int output) override;
};

// Class to interface Arduino's delay and elapsed time functions
class Arduino_delay : public Arduino_interface_delay
{
public:
    ~Arduino_delay(){};

public:
    unsigned long milli_seconds(void) override;
    unsigned long micro_seconds(void) override;
    void delay_ms(unsigned long ms) override;
    void delay_us(unsigned int us) override;
};

// Class to interface Arduino's com port and print out information
// All info taken from "USBAPI.h" & "CDC.cpp"
class Arduino_com : public Arduino_interface_com
{
public:
    ~Arduino_com(){};

public:
    void com_begin(unsigned long baudrate) override;
    void com_print(uint8_t i) override;
    void com_println(const char *str) override;
};

class Arduino_random : public Arduino_interface_random
{
public:
    ~Arduino_random(){};
    uint8_t random_generateUi8();

private:
    void random_generateSeed(byte floatingAnalogPin_Id);
};

class Arduino_eeprom : public Arduino_interface_eeprom
{
public:
    uint8_t eeprom_read(uint8_t memId);
    void eeprom_write(uint8_t memId, uint8_t contents);
};

#endif // ARDUINO_IMPLEMENTATION_H