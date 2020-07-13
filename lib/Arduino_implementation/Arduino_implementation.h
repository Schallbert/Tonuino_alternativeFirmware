#ifndef ARDUINO_IMPLEMENTATION_H
#define ARDUINO_IMPLEMENTATION_H
#include <Arduino.h>
#include <Arduino_types.h>
#include <Arduino_interface.h>

class Arduino_pins : public Arduino_interface_pins
{
public:
    ~Arduino_pins(){};

public:
    void pin_mode(uint8_t pinId, uint8_t mode) override
    {
        pinMode(pinId, mode);
    };
    void digital_write(uint8_t pinId, uint8_t output) override
    {
        digitalWrite(pinId, output);
    };
    int digital_read(uint8_t pinId) override
    {
        return digitalRead(pinId);
    };
    int analog_read(uint8_t pinId) override
    {
        return analogRead(pinId);
    };
    void analog_reference(uint8_t mode) override
    {
        analogReference(mode);
    };
    void analog_write(uint8_t pinId, int output) override
    {
        analogWrite(pinId, output);
    };
};

// Class to interface Arduino's delay and elapsed time functions
class Arduino_delay : Arduino_interface_delay
{
public:
    ~Arduino_interface_delay(){};

public:
    unsigned long milli_seconds(void) override
    {
        return millis();
    }
    unsigned long micro_seconds(void) override
    {
        return micros();
    }
    void delay_ms(unsigned long ms) override
    {
        delay(ms);
    }
    void delay_us(unsigned int us) override
    {
        delayMicroseconds(us);
    }
};

// Class to interface Arduino's com port and print out information
// All info taken from "USBAPI.h" & "CDC.cpp"
class Arduino_com : Arduino_interface_com
{
public:
    ~Arduino_interface_serial(){};

public:
    void com_begin(unsigned long baudrate) override
    {
        Serial.begin(baudrate);
    }
    void com_print(uint8_t i) override
    {
        Serial.print(i);
    }
    void com_println(const char* str) override
    {
        Serial.println(str);
    }
};

#endif // ARDUINO_IMPLEMENTATION_H