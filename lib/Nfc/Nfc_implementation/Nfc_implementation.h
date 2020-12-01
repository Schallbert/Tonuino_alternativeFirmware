#ifndef NFC_IMPLEMENTATION_H
#define NFC_IMPLEMENTATION_H

#include "Arduino_config.h"
#include "../MFRC522/MFRC522_interface.h" // to access actual HW
#include "../Nfc_interface/Nfc_interface.h"
#include "../NfcTag_implementation/NfcTag_factory.h"
#include "../MessageHandler/MessageHandler_interface.h"

class NfcNotify
{
public:
    enum eNfcNotify
    {
        noMessage = 0,
        tagWriteSuccess,
        tagReadSuccess,
        tagWriteError,
        tagReadError,
        tagTypeNotImplementedError
    };

    static const char *toString(eNfcNotify value)
    {
#if DEBUGSERIAL
        static const char *NOTIFY_STRING[] = {
            nullptr,
            "Tag Write Success",
            "Tag Read Success",
            "Tag write Error",
            "Tag read Error",
            "Tag type unknown/not implemented"
            };
        return NOTIFY_STRING[value];
#endif
        return nullptr;
    }
};

// forwards power up etc. directly to MFRC interface
// channels read/write requests to downstream NfcTag objects.
class Nfc_implementation : public Nfc_interface
{
public:
    Nfc_implementation(MFRC522_interface *pMfrc522,
                       MessageHander_interface *pMessageHandler) : m_pMfrc522(pMfrc522),
                                                                   m_pMessageHandler(pMessageHandler){};
    ~Nfc_implementation(){};

public:
    void initNfc() override;
    Nfc_interface::eTagState getTagPresence(void) override;
    bool writeTag(byte blockAddress, byte *dataToWrite) override;
    bool readTag(byte blockAddress, byte *readResult) override;

private:
    // Halts communication to card and stops crypto methods
    void setTagOffline();
    // Returns true if communications to a card is successfully established
    bool setTagOnline();
    // gets concrete, fitting tag instance from factory
    bool getTag();
    // Helper method, for better readability: takes status of function and returns input Notification
    void setNotification(bool status, NfcNotify::eNfcNotify sucessMessage, NfcNotify::eNfcNotify failureMessage);

private:
    MFRC522_interface *m_pMfrc522{nullptr};
    MessageHander_interface *m_pMessageHandler{nullptr};

    NfcTag_interface *m_pConcreteTag{nullptr};
};
#endif // NFC_IMPLEMENTATION_H