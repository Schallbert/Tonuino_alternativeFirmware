#ifndef MFRC522_INTERFACE_H
#define MFRC522_INTERFACE_H

#include "Arduino_types.h"

class MFRC522_interface
{
public:
    // simply compied from MFRC522.h :/
    enum eTagType : byte
    {
        PICC_TYPE_UNKNOWN,
        PICC_TYPE_ISO_14443_4,        // PICC compliant with ISO/IEC 14443-4
        PICC_TYPE_ISO_18092,          // PICC compliant with ISO/IEC 18092 (NFC)
        PICC_TYPE_MIFARE_MINI,        // MIFARE Classic protocol, 320 bytes
        PICC_TYPE_MIFARE_1K,          // MIFARE Classic protocol, 1KB
        PICC_TYPE_MIFARE_4K,          // MIFARE Classic protocol, 4KB
        PICC_TYPE_MIFARE_UL,          // MIFARE Ultralight or Ultralight C
        PICC_TYPE_MIFARE_PLUS,        // MIFARE Plus
        PICC_TYPE_MIFARE_DESFIRE,     // MIFARE DESFire
        PICC_TYPE_TNP3XXX,            // Only mentioned in NXP AN 10833 MIFARE Type Identification Procedure
        PICC_TYPE_NOT_COMPLETE = 0xff // SAK indicates UID is not complete.
    };

public:
    virtual ~MFRC522_interface(){};

public:
    // initializes communication to device
    virtual void init() = 0;
    // power control methods
    virtual void softPowerDown() = 0;
    virtual void softPowerUp() = 0;
    // set tag to "sleep" and halt communication
    virtual void tagHalt() = 0;
    // end communication with tag
    virtual void tagLogoff() = 0;
    // system supports multiple manufacturers and tag sizes
    virtual eTagType getTagType() = 0;

    // Helper methods
    // returns true ONCE when a "fresh" tag is detected
    virtual bool isNewCardPresent() = 0;
    // returns true if a tag is detected
    virtual bool isCardPresent() = 0;

    // ------------------------------------------------------------------
    // BELOW METHODS ARE SPECIFIC AND DIFFER BETWEEN DIFFERENT TAG TYPES!
    // authenticate NfcTag to reader, blockAddress selects memory sector to authenticate
    virtual bool tagLogin(byte blockAddress) = 0;
    // reads data from tag's specific block Address into buffer
    virtual bool tagRead(byte blockAddress, byte *buffer, byte *bufferSize) = 0;
    // writes data from buffer to specific block addres of tag
    virtual bool tagWrite(byte blockAddress, byte *buffer, byte bufferSize) = 0;
};

#endif // MFRC522_INTERFACE_H