#ifndef ARDUINO_IMPLEMENTATION_H
#define ARDUINO_IMPLEMENTATION_H

#include "../Arduino_HardwareAbstraction/Arduino_interface.h"

class Arduino_pins : public Arduino_interface_pins
{
public:
    Arduino_pins() = default;
    ~Arduino_pins() = default;
    Arduino_pins(const Arduino_pins &cpy) = delete;

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
    Arduino_delay() = default;
    ~Arduino_delay() = default;
    Arduino_delay(const Arduino_delay &cpy) = delete;


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
    Arduino_com() = default;
    ~Arduino_com() = default;
    Arduino_com(const Arduino_com &cpy) = delete;

    void com_begin(unsigned long baudrate) override;
    void com_print(uint8_t i) override;
    void com_println(const char *str) override;
};

class Arduino_random : public Arduino_interface_random
{
public:
    Arduino_random() = default;
    ~Arduino_random() = default;
    Arduino_random(const Arduino_random &cpy) = delete;

    uint8_t random_generateUi8() override;

private:
    void random_generateSeed(byte floatingAnalogPin_Id)  override;
};

class Arduino_eeprom : public Arduino_interface_eeprom
{
public:
    Arduino_eeprom() = default;
    ~Arduino_eeprom() = default;
    Arduino_eeprom(const Arduino_eeprom &cpy) = delete;

    uint8_t eeprom_read(uint8_t memId)  override;
    void eeprom_write(uint8_t memId, uint8_t contents)  override;
};

#endif // ARDUINO_IMPLEMENTATION_H