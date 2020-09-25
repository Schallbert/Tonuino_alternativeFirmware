#ifndef MFRC522_INTERFACE_H
#define MFRC522_INTERFACE_H
#include "Arduino_types.h"

// Wrapper class to separate dependency MFRC522 card reader from utilizing classes.
// Enabler for mocking MFRC522 behavior.
class MFRC522_interface
{

public:
    virtual ~MFRC522_interface(){};

public:
    // Inits communication to card reader
    virtual void initReader(void) = 0;
    // Returns true if a card can be detected on reader
    virtual bool isCardPresent(void) = 0;
    // Returns true if a card that is not the currently active one
    // is present on the reader
    virtual bool isNewCardPresent(void) = 0;
    // Returns true on successful write of a 16byte chunk
    // of data to a sector->block of the card
    virtual bool writeCard(byte blockAddr, byte *dataToWrite) = 0;
    // Returns true on successful read of a 16byte chunk
    // of data to a sector->block of the card
    virtual bool readCard(byte blockAddr, byte *readResult) = 0;
    // Debug function. Returns notification from NFC reader system.
    virtual const char *checkMFRC522Notification() = 0;

public:
    static const uint8_t NFCTAG_MEMORY_TO_OCCUPY = 16; //bytes
};
#endif // MFRC522_INTERFACE_H