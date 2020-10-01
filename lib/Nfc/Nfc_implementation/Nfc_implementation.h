#ifndef NFC_IMPLEMENTATION_H
#define NFC_IMPLEMENTATION_H

#include "Arduino_config.h"
#include "../MFRC522/MFRC522_interface.h" // to access actual HW
#include "../Nfc_interface/Nfc_interface.h"
#include "../NfcTag_implementation/NfcTag_factory.h"

// forwards power up etc. directly to MFRC interface
// channels read/write requests to downstream NfcTag objects.
class Nfc_implementation : public Nfc_interface
{
public:
    enum eNfcNotify
    {
        noMessage = 0,
        tagWriteSuccess,
        tagReadSuccess,
        tagWriteError,
        tagReadError,
        tagTypeNotImplementedError,
        tagSetOnlineFailed,
        tagRequestOutOfRangeWwrning
    };

public:
    Nfc_implementation(MFRC522_interface *pMfrc522) : m_pMfrc522(pMfrc522){};
    ~Nfc_implementation() { NfcTag_factory::removeInstance(); };

public:
    void initNfc() override;
    Nfc_interface::eTagState getTagPresence(void) override;
    bool writeTag(byte blockAddr, byte *dataToWrite) override;
    bool readTag(byte blockAddr, byte *readResult) override;
    const char *getNfcNotification() override;

private:
    // Halts communication to card and stops crypto methods
    void setTagOffline();
    // Returns true if communications to a card is successfully established
    bool setTagOnline();
    // Helper method, writes tag type to instance variable
    bool getTagType();
    // Helper method, for debugging. Sends message string to requesting entity
    static inline const char *stringFromNfcNotify(eNfcNotify value);
    // Helper method, for better readability: takes status of function and returns input Notification
    void setNotification(bool status, eNfcNotify sucessMessage, eNfcNotify failureMessage);

private:
    MFRC522_interface *m_pMfrc522{nullptr};
    MFRC522_interface::eTagType m_tagType{MFRC522_interface::PICC_TYPE_UNKNOWN};
    eNfcNotify m_eNotification{noMessage};
};
#endif // NFC_IMPLEMENTATION_H