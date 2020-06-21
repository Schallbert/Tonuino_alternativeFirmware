#ifndef EEPROMIMPLEMENTATION_H
#define EEPROMIMPLEMENTATION_H

#include <Arduino.h>
#include "EEPROM_interface.h"
#include "EEPROM.h"

// Implementation of interface.
class Eeprom : public EEPROM_interface
{
    public:
    uint8_t read(uint8_t memId)
    { 
        return EEPROM.read((int)memId);
    }
    void write(uint8_t memId, uint8_t contents)
    {
        EEPROM.update((int)memId, contents);
    }
};

#endif // EEPROMIMPLEMENTATION_H