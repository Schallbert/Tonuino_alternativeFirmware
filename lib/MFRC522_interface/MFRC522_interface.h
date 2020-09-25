#ifndef MFRC522_INTERFACE_H
#define MFRC522_INTERFACE_H
#include "Arduino_types.h"

// Wrapper class to separate dependency MFRC522 card reader from utilizing classes.
// Enabler for mocking MFRC522 behavior.
class MFRC522_interface
{
public:
    enum eTagState
    {
        NO_CARD = 0,       // allow certain actions (help, deleteCard etc)
        ACTIVE_KNOWN_CARD, // full playback
        NEW_KNOWN_CARD,    // read card, get folder, full playback
        UNKNOWN_CARD_MENU, // play voice menu, link folder to card
        DELETE_CARD_MENU,  // delete card menu
        NUMBER_OF_CARD_STATES
    };

public:
    virtual ~MFRC522_interface(){};

public:
    // returns state of
    virtual MFRC522_interface::eTagState getTagPresence(void) = 0;
    virtual bool writeTag(byte blockAddr, byte *dataToWrite) = 0;
    // Returns true on successful read of a 16byte chunk
    // of data to a sector->block of the card
    virtual bool readTag(byte blockAddr, byte *readResult) = 0;
    // Debug function. Returns notification from NFC reader system.
    virtual const char *checkMFRC522Notification() = 0;

public:
    static const uint8_t NFCTAG_MEMORY_TO_OCCUPY = 16; //bytes
};
#endif // MFRC522_INTERFACE_H