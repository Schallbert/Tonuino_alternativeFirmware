#ifndef NFCTAG_INTERFACE_H
#define NFCTAG_INTERFACE_H

#include "Arduino_types.h"
#include "Tonuino_config.h"
#include "../MFRC522/MFRC522_interface.h"

class NfcTag_interface
{

public:
    virtual ~NfcTag_interface(){};

public:
    // Read data from block for implemented NFC types
    virtual bool readTag(byte blockAddress, byte *readResult) = 0;
    // Write data to block for for implemented NFC types
    virtual bool writeTag(byte blockAddress, byte *dataToWrite) = 0;

private:
    virtual void checkAndRectifyBlockAddress(byte &blockAddress) = 0;
};

#endif // NFCTAG_INTERFACE_H