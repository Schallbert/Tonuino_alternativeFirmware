#ifndef ARDUINO_INTERFACE_H
#define ARDUINO_INTERFACE_H
#include "../Arduino/Arduino_interface/Arduino_types.h"

    // Interface class for Arduino pin manipulation functions
    // Preparation for mocking
    class Arduino_interface_pins
{
public:
    virtual ~Arduino_interface_pins(){};

public:
    virtual void pin_mode(uint8_t pinId, uint8_t mode) = 0;
    virtual void digital_write(uint8_t pinId, uint8_t output) = 0;
    virtual int digital_read(uint8_t pinId) = 0;
    virtual int analog_read(uint8_t pinId) = 0;
    virtual void analog_reference(uint8_t mode) = 0;
    virtual void analog_write(uint8_t pinId, int output) = 0;
};

// Class to interface Arduino's delay and elapsed time functions
class Arduino_interface_delay
{
public:
    virtual ~Arduino_interface_delay(){};

public:
    virtual unsigned long milli_seconds(void) = 0;
    virtual unsigned long micro_seconds(void) = 0;
    virtual void delay_ms(unsigned long ms) = 0;
    virtual void delay_us(unsigned int us) = 0;
};

// Class to interface Arduino's com port and print out information
// All info taken from "USBAPI.h" & "CDC.cpp"
class Arduino_interface_com
{
public:
    virtual ~Arduino_interface_com(){};

public:
    virtual void com_begin(unsigned long baudrate) = 0;
    virtual void com_print(uint8_t i) = 0;
    virtual void com_println(const char *str) = 0;
};

class Arduino_interface_random
{
public:
    virtual ~Arduino_interface_random(){};

public:
    // generates a seed for the random generator to use.
    // utilizes floating analog pin for noise input.
    // If input is 0, seed will use a constant and the generator
    // will always output the exact same sequence if asked.
    virtual void random_generateSeed(byte floatingAnalogPin_Id) = 0;
    // returns a pseudo random value between 0 and 255.
    // Don't forget to call generateSeed first.
    virtual uint8_t random_generateUi8() = 0;
};

class Arduino_interface_eeprom
{
public:
    virtual ~Arduino_interface_eeprom(){};

public:
    virtual uint8_t eeprom_read(uint8_t memId) = 0;
    virtual void eeprom_write(uint8_t memId, uint8_t contents) = 0;
};
#endif //ARDUINO_INTERFACE_H