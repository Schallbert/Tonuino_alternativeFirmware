#ifndef ARDUINO_INTERFACE_H
#define ARDUINO_INTERFACE_H
#include <Arduino_types.h>

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
#endif //ARDUINO_INTERFACE_H