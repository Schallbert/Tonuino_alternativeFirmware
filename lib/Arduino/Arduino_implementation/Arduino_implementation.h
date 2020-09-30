#ifndef ARDUINO_IMPLEMENTATION_H
#define ARDUINO_IMPLEMENTATION_H

#include "Arduino.h"
#include "EEPROM.h"
#include "../Arduino/Arduino_interface/Arduino_interface.h"

class Arduino_pins : public Arduino_interface_pins
{
public:
    ~Arduino_pins(){};

public:
    void pin_mode(uint8_t pinId, uint8_t mode) override
    {
        pinMode(pinId, mode);
    }
    void digital_write(uint8_t pinId, uint8_t output) override
    {
        digitalWrite(pinId, output);
    }
    int digital_read(uint8_t pinId) override
    {
        return digitalRead(pinId);
    }
    int analog_read(uint8_t pinId) override
    {
        return analogRead(pinId);
    }
    void analog_reference(uint8_t mode) override
    {
        analogReference(mode);
    }
    void analog_write(uint8_t pinId, int output) override
    {
        analogWrite(pinId, output);
    }
};

// Class to interface Arduino's delay and elapsed time functions
class Arduino_delay : public Arduino_interface_delay
{
public:
    ~Arduino_delay(){};

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
class Arduino_com : public Arduino_interface_com
{
public:
    ~Arduino_com(){};

public:
    void com_begin(unsigned long baudrate) override
    {
        Serial.begin(baudrate);
    }
    void com_print(uint8_t i) override
    {
        Serial.print(i);
    }
    void com_println(const char *str) override
    {
        Serial.println(str);
    }
};

class Arduino_random : public Arduino_interface_random
{
public:
    uint8_t random_generateUi8()
    {
        return random(0xFF);
    }

private:
    void random_generateSeed(byte floatingAnalogPin_Id)
    {
        if (floatingAnalogPin_Id == 0)
        {
            randomSeed(1);
        }
        else
        {
            uint32_t ADC_LSB;
            uint32_t ADCSeed;
            for (uint8_t i = 0; i < 128; i++)
            {
                ADC_LSB = analogRead(floatingAnalogPin_Id) & 0x1;
                ADCSeed ^= ADC_LSB << (i % 32);
            }
            randomSeed(ADCSeed); // Init Arduino random generator
        }
    }
};

class Arduino_eeprom : public Arduino_interface_eeprom
{
public:
    uint8_t eeprom_read(uint8_t memId)
    {
        return EEPROM.read(static_cast<int>(memId));
    }
    void eeprom_write(uint8_t memId, uint8_t contents)
    {
        EEPROM.update(static_cast<int>(memId), contents);
    }
};

#endif // ARDUINO_IMPLEMENTATION_H