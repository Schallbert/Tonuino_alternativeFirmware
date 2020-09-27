#ifndef NFCTAG_INTERFACE_H
#define NFCTAG_INTERFACE_H

#include "Arduino_types.h"

enum eMFRC522Notify;

class NfcTag_interface
{
public:
    virtual ~NfcTag_interface(){};

public:
    virtual bool readTag(byte blockAddr, byte *readResult) = 0;
    virtual bool writeTag(byte blockAddr, byte *dataToWrite) = 0;
    // Debug function. Returns notification from NFC reader system.
    virtual eMFRC522Notify getMFRC522Notification() = 0;

private:
    virtual bool authenticateCard() = 0;
    virtual void checkAndRectifyBlockAddress(byte &blockAddress) = 0;
};

#endif // NFCTAG_INTERFACE_H