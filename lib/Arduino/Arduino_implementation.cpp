
#include "Arduino.h"
#include "EEPROM.h"

#include "Arduino_implementation.h"

void Arduino_pins::pin_mode(uint8_t pinId, uint8_t mode)
{
    pinMode(pinId, mode);
}

void Arduino_pins::digital_write(uint8_t pinId, uint8_t output)
{
    digitalWrite(pinId, output);
}

int Arduino_pins::digital_read(uint8_t pinId)
{
    return digitalRead(pinId);
}

int Arduino_pins::analog_read(uint8_t pinId)
{
    return analogRead(pinId);
}

void Arduino_pins::analog_reference(uint8_t mode)
{
    analogReference(mode);
}

void Arduino_pins::analog_write(uint8_t pinId, int output)
{
    analogWrite(pinId, output);
}

unsigned long Arduino_delay::milli_seconds(void)
{
    return millis();
}

unsigned long Arduino_delay::micro_seconds(void)
{
    return micros();
}

void Arduino_delay::delay_ms(unsigned long ms)
{
    delay(ms);
}

void Arduino_delay::delay_us(unsigned int us)
{
    delayMicroseconds(us);
}

void Arduino_com::com_begin(unsigned long baudrate)
{
    Serial.begin(baudrate);
    while (!Serial); // wait for Serial to be initiated
    Serial.print("Serial @");
    Serial.println(baudrate);
}

void Arduino_com::com_print(uint8_t i)
{
    Serial.print(i);
}

void Arduino_com::com_println(const char *str)
{
    Serial.println(str);
}

uint8_t Arduino_random::random_generateUi8()
{
    return random(0xFF);
}

void Arduino_random::random_generateSeed(byte floatingAnalogPin_Id)
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

uint8_t Arduino_eeprom::eeprom_read(uint8_t memId)
{
    return EEPROM.read(static_cast<int>(memId));
}

void Arduino_eeprom::eeprom_write(uint8_t memId, uint8_t contents)
{
    EEPROM.update(static_cast<int>(memId), contents);
}