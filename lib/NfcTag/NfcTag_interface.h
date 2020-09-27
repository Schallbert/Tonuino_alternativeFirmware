#ifndef NFCTAG_INTERFACE_H
#define NFCTAG_INTERFACE_H

#include "Arduino_types.h"
#include "MFRC522.h"

class NfcTag_interface
{
public:
    enum eMFRC522Notify
    {
        noMessage = 0,
        tagOnline,
        tagOffline,
        errorTagWrite,
        errorTagRead,
        errorTagSetOnlineFailed,
        errorTagAuthenticateFailed,
        errorTagRequestOutOfRange
    };

public:
    virtual ~NfcTag_interface(){};

public:
    // Read data from block for implemented NFC types
    virtual bool readTag(byte blockAddress, byte *readResult) = 0;
    // Write data to block for for implemented NFC types
    virtual bool writeTag(byte blockAddress, byte *dataToWrite) = 0;
    // Debug function. Returns notification from NFC reader system.
    virtual eMFRC522Notify getNfcNotification() = 0;

private:
    virtual bool authenticateTag() = 0;
    virtual void checkAndRectifyBlockAddress(byte &blockAddress) = 0;

private:
    MFRC522::MIFARE_Key m_eKey = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}; // 6 byte key, factory default all set.
};

#endif // NFCTAG_INTERFACE_H