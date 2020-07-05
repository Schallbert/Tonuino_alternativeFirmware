#ifndef MFRC522_IMPLEMENTATION_H
#define MFRC522_IMPLEMENTATION_H

#include <Arduino.h>
#include <Defines.h>
#include <MFRC522.h>
#include "MFRC522_interface.h"

class Mfrc522 : public MFRC522_interface
{
public:
    void initReader();
    bool isCardPresent(void);
    bool isNewCardPresent(void);
    bool writeCard(byte blockAddr, byte *dataToWrite);
    bool readCard(byte blockAddr, byte *readResult);

private:
    // Write data to block for for different NFC types
    MFRC522::StatusCode writeMini1k4k(byte blockAddr, byte* data);
    MFRC522::StatusCode writeUltraLight(byte blockAddr, byte* data);
    // Read data from block for different NFC types
    MFRC522::StatusCode readMini1k4k(byte blockAddr, byte* result);
    MFRC522::StatusCode readUltraLight(byte blockAddr, byte* result);
    // Halts communication to card and stops crypto functions
    void setCardOffline();
    // Returns true if communications to a card is successfully established
    bool setCardOnline();
    // Returns true if card authentication succeeds
    bool authenticateMini1k4k();
    bool authenticateUltraLight();
    // Helper function, writes tag type to instance variable
    bool getTagType();
    // Helper function that limites block address to memory range
    void checkBlockAddressMini1k4k(byte &blockAddress);
    void checkBlockAddressUltraLight(byte &blockAddress);
    
private:
    MFRC522 m_pMfrc522{MFRC522(SS_PIN, RST_PIN)};
    MFRC522::PICC_Type m_tagType{MFRC522::PICC_TYPE_UNKNOWN};
    MFRC522::MIFARE_Key m_eKey = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}; // 6 byte key, factory default all set.
    static const byte MIFARE_UL_BLOCK_SIZE {4};
    const byte ULTRALIGHTSTARTPAGE {4};
    const byte ULTRALIGHTSTOPPAGE {11};
    byte m_ui8SectorMini1k4k = {0};
    byte m_ui8TrailerBlockMini1k4k {3};
    const byte SECTORSTRAILERBLOCKMINI1K4K {3};
};
#endif // MFRC522_IMPLEMENTATION_H