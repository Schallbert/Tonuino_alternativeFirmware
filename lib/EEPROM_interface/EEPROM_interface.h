#ifndef EEPROM_INTERFACE_H
#define EEPROM_INTERFACE_H
#include <Arduino_types.h>

// Wrapper class to separate dependency EEPROM from utilizing classes.
// Enabler for mocking EEPROM behavior.
class EEPROM_interface
{
    public:
    virtual ~EEPROM_interface() {};

    public:
    virtual uint8_t read(uint8_t memId) = 0;
    virtual void write(uint8_t memId, uint8_t contents) = 0;
};
#endif // EEPROM_INTERFACE_H