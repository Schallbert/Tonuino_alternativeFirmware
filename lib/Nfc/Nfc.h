#ifndef NFC_H
#define NFC_H

#include "Arduino_types.h"
#include "Arduino_config.h"
#include "MFRC522.h"
#include "Nfc_interface.h"

// move card presence enum down to here
// isCardPresent to return this enum
// make implementation testable

// outsource card types to an own calss
// 

class Nfc : public Nfc_interface
{

public:
    void initNfc() override;
    Nfc_interface::eTagState getTagPresence(void) override;
    bool writeTag(byte blockAddr, byte *dataToWrite) override;
    bool readTag(byte blockAddr, byte *readResult) override;
    const char *getNfcNotification() override;

private:
    // Halts communication to card and stops crypto methods
    void setCardOffline();
    // Returns true if communications to a card is successfully established
    bool setCardOnline();
    // Helper method, writes tag type to instance variable
    bool getTagType();
    // Helper method, for debugging. Sends message string to requesting entity
    static inline const char *stringFromNfcNotify(eMFRC522Notify value);

private:
    MFRC522 m_pMfrc522{MFRC522(SS_PIN, RST_PIN)};
    MFRC522::PICC_Type m_tagType{MFRC522::PICC_TYPE_UNKNOWN};
    
    eMFRC522Notify m_eNotification{noMessage};
};
#endif // NFC_H