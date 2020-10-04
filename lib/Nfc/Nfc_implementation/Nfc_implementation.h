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
        tagRequestOutOfRangeWarning,
        number_of_notifications = 7
    };

public:
    Nfc_implementation(MFRC522_interface *pMfrc522) : m_pMfrc522(pMfrc522){};
    ~Nfc_implementation(){};

public:
    void initNfc() override;
    Nfc_interface::eTagState getTagPresence(void) override;
    bool writeTag(byte blockAddress, byte *dataToWrite) override;
    bool readTag(byte blockAddress, byte *readResult) override;
    const char *getNfcNotification() override;

private:
    // Halts communication to card and stops crypto methods
    void setTagOffline();
    // Returns true if communications to a card is successfully established
    bool setTagOnline();
    // gets concrete, fitting tag instance from factory
    bool getTag();
    // Helper method, for debugging. Sends message string to requesting entity
    static inline const char *stringFromNfcNotify(eNfcNotify value);
    // Helper method, for better readability: takes status of function and returns input Notification
    void setNotification(bool status, eNfcNotify sucessMessage, eNfcNotify failureMessage);

private:
    MFRC522_interface *m_pMfrc522{nullptr};
    NfcTag_interface *m_pConcreteTag{nullptr};
    eNfcNotify m_eNotification{noMessage};
};
#endif // NFC_IMPLEMENTATION_H