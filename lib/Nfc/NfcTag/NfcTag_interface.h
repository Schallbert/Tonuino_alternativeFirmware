#ifndef NFCTAG_INTERFACE_H
#define NFCTAG_INTERFACE_H

#include "../Arduino/Arduino_types.h"

class NfcTag_interface
{

public:
    virtual ~NfcTag_interface(){};

public:
    // Read data from block for implemented NFC types
    virtual bool readTag(byte blockAddress, byte *readResult) = 0;
    // Write data to block for for implemented NFC types
    virtual bool writeTag(byte blockAddress, byte *dataToWrite) = 0;

protected:
    // Cheap copy of c-function "memcpy". Due to interface logic it is easier
    // to re-implement it here...
    static void copyArray(byte *target, byte *source, uint8_t numberOfBytes)
    {
        for (int i = 0; i < numberOfBytes; i++)
        {
            *(target + i) = *(source + i);
        }
    };

private:
    virtual void checkAndRectifyBlockAddress(byte &blockAddress) = 0;
};

#endif // NFCTAG_INTERFACE_H