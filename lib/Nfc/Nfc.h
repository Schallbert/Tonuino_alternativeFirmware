#ifndef NFC_H
#define NFC_H

#include "Arduino_config.h"
#include "../MFRC522/MFRC522_interface.h" // to access actual HW
#include "../Nfc_interface/Nfc_interface.h"
#include "../NfcTag/NfcTag_factory.h"

// move card presence enum down to here
// isCardPresent to return this enum
// make implementation testable

// outsource card types to an own calss
//

class Nfc : public Nfc_interface
{
public:
    enum eNfcNotify
    {
        noMessage = 0,
        tagOnline,
        tagOffline,
        errorTagWrite,
        errorTagRead,
        errorTagTypeNotImplemented,
        errorTagSetOnlineFailed,
        warningTagRequestOutOfRange
    };

public:
    Nfc(MFRC522_interface *pMfrc522) : m_pMfrc522(pMfrc522){};
    ~Nfc() { NfcTag_interface::removeInstance(); };

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
    static inline const char *stringFromNfcNotify(eNfcNotify value);

private:
    MFRC522_interface *m_pMfrc522{nullptr};
    MFRC522_interface::Tag_Type m_tagType{MFRC522_interface::PICC_TYPE_UNKNOWN};
    eNfcNotify m_eNotification{noMessage};
};
#endif // NFC_H