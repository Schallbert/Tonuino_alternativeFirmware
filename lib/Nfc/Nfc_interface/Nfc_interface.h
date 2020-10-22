#ifndef NFC_INTERFACE_H
#define NFC_INTERFACE_H
#include "../Arduino/Arduino_interface/Arduino_types.h"

// Wrapper class to separate dependency MFRC522 card reader from utilizing classes.
// Enabler for mocking MFRC522 behavior.
class Nfc_interface
{
public:
    enum eTagState
    {
        NO_TAG = 0,       // 0 allow certain actions (help, deleteCard etc)
        ACTIVE_KNOWN_TAG, // 1 full playback
        NEW_KNOWN_TAG,    // 2 read card, get folder, full playback
        NEW_UNKNOWN_TAG,  // 3 play voice menu, link folder to card
        DELETE_TAG_MENU,  // 4 delete card menu
        NUMBER_OF_TAG_STATES = 5
    };

public:
    virtual ~Nfc_interface(){};

public:
    // initializes the NFC reader (serial etc.)
    virtual void initNfc() = 0;
    // returns presence state of NFC tag
    virtual Nfc_interface::eTagState getTagPresence(void) = 0;
    // Returns true on successful write of data to a sector->block of the tag
    virtual bool writeTag(byte blockAddress, byte *dataToWrite) = 0;
    // Returns true on successful read
    // of data to a sector->block of the tag
    virtual bool readTag(byte blockAddress, byte *readResult) = 0;
    // Debug function. Returns notification from NFC reader system.
    virtual const char *getNfcNotification() = 0;
};
#endif // NFC_INTERFACE_H