#ifndef MFRC522_IMPLEMENTATION_H
#define MFRC522_IMPLEMENTATION_H

#include "Arduino_types.h"
#include "Arduino_config.h"
#include "MFRC522.h"
#include "MFRC522_interface.h"

// move card presence enum down to here
// isCardPresent to return this enum
// make implementation testable

// outsource card types to an own calss
// 

class Mfrc522 : public MFRC522_interface
{

public:
    void initNfc() override;
    MFRC522_interface::eTagState getTagPresence(void) override;
    bool writeTag(byte blockAddr, byte *dataToWrite) override;
    bool readTag(byte blockAddr, byte *readResult) override;
    const char *getMFRC522Notification() override;

private:
    // Write data to block for for different NFC types
    MFRC522::StatusCode writeMini1k4k(byte blockAddr, byte *data);
    MFRC522::StatusCode writeUltraLight(byte blockAddr, byte *data);
    // Read data from block for different NFC types
    MFRC522::StatusCode readMini1k4k(byte blockAddr, byte *result);
    MFRC522::StatusCode readUltraLight(byte blockAddr, byte *result);
    // Halts communication to card and stops crypto methods
    void setCardOffline();
    // Returns true if communications to a card is successfully established
    bool setCardOnline();
    // Returns true if card authentication succeeds
    bool authenticateMini1k4k();
    bool authenticateUltraLight();
    // Helper method, writes tag type to instance variable
    bool getTagType();
    // Helper method that limites block address to memory range
    void checkBlockAddressMini1k4k(byte &blockAddress);
    void checkBlockAddressUltraLight(byte &blockAddress);
    // Helper method, for debugging. Sends message string to requesting entity
    static inline const char *stringFromMFRC522Notify(eMFRC522Notify value);

private:
    MFRC522 m_pMfrc522{MFRC522(SS_PIN, RST_PIN)};
    MFRC522::PICC_Type m_tagType{MFRC522::PICC_TYPE_UNKNOWN};
    MFRC522::MIFARE_Key m_eKey = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}; // 6 byte key, factory default all set.
    static const byte MIFARE_UL_BLOCK_SIZE{4};
    static const byte ULTRALIGHTSTARTPAGE{4};
    static const byte ULTRALIGHTSTOPPAGE{11};
    static const byte SECTORSTRAILERBLOCKMINI1K4K{3};
    byte m_ui8SectorMini1k4k = {0};
    byte m_ui8TrailerBlockMini1k4k{3};
    
    eMFRC522Notify m_eNotification{noMessage};
};
#endif // MFRC522_IMPLEMENTATION_H