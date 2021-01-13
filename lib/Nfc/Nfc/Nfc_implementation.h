#ifndef NFC_IMPLEMENTATION_H
#define NFC_IMPLEMENTATION_H

#include "../MFRC522/MFRC522_interface.h"
#include "../MessageHandler/MessageHandler_interface.h"
#include "Tonuino_config.h"

#include "../NfcTag/NfcTag_factory.h"
#include "../Nfc/Nfc_interface.h"

// forwards power up etc. directly to MFRC interface
// channels read/write requests to downstream NfcTag objects.
class Nfc_implementation : public Nfc_interface
{
public:
    Nfc_implementation(MFRC522_interface &rMfrc522,
                       MessageHander_interface &rMessageHandler) : m_rMfrc522(rMfrc522),
                                                                   m_rMessageHandler(rMessageHandler){};
    ~Nfc_implementation(){};

public:
    void initNfc() override;
    Message::eMessageContent getTagPresence() override;
    bool writeTag(byte blockAddress, byte *dataToWrite) override;
    bool readTag(byte blockAddress, byte *readResult) override;

private:
    // Halts communication to card and stops crypto methods
    void setTagOffline();
    // Returns true if communications to a card is successfully established
    bool setTagOnline();
    // Helper method, for better readability: takes status of function and returns input Notification
    void printNotification(bool status, Message::eMessageContent sucessMessage, Message::eMessageContent failureMessage);

private:
    MFRC522_interface &m_rMfrc522;
    MessageHander_interface &m_rMessageHandler;

    NfcTag_factory m_NfcTagFactory;
    NfcTag_interface *m_pConcreteTag{nullptr};
};
#endif // NFC_IMPLEMENTATION_H