#include "Arduino_config.h"
#include "../NfcTag/NfcTag_factory.h"
#include "Nfc_implementation.h"

NfcControl_interface::eTagState Nfc_implementation::getTagPresence()
{
    NfcControl_interface::eTagState returnValue{NfcControl_interface::NO_TAG};
    if (m_rMfrc522.isCardPresent())
    {
        m_rMessageHandler.printMessage("DEBUG:TagDetected!"); // TODO: remove!
        // A card is present!
        if (!m_rMfrc522.isNewCardPresent())
        {
            returnValue = NfcControl_interface::ACTIVE_KNOWN_TAG;
        }
        else
        {
            // New card detected: runs once as new card is automatically set to ActiveCard
            if (setTagOnline())
            {
                returnValue = NfcControl_interface::NEW_UNKNOWN_TAG; // assume tag is unknown
            }
        }
    }
    return returnValue;
}

void Nfc_implementation::initNfc()
{
    m_rMfrc522.init(); // Init MFRC522
    m_rMessageHandler.printMessage(NfcNotify::toString(NfcNotify::tagReaderInit));
}

bool Nfc_implementation::writeTag(byte blockAddress, byte *dataToWrite)
{
    bool status{false};
    if (setTagOnline())
    {
        status = m_pConcreteTag->writeTag(blockAddress, dataToWrite);
    }
    setTagOffline();
    printNotification(status, NfcNotify::tagWriteSuccess, NfcNotify::tagWriteError);
    return status;
}

bool Nfc_implementation::readTag(byte blockAddress, byte *readResult)
{
    bool status{false};
    if (setTagOnline())
    {
        status = m_pConcreteTag->readTag(blockAddress, readResult);
    }
    setTagOffline();
    printNotification(status, NfcNotify::tagReadSuccess, NfcNotify::tagReadError);
    return status;
}

void Nfc_implementation::printNotification(bool status, NfcNotify::eNfcNotify successMessage, NfcNotify::eNfcNotify failureMessage)
{
    static NfcNotify::eNfcNotify message{NfcNotify::noMessage};
    NfcNotify::eNfcNotify newMessage{NfcNotify::noMessage};
    if (status)
    {
        newMessage = successMessage;
    }
    else
    {
        newMessage = failureMessage;
    }

    if (newMessage != NfcNotify::noMessage &&
        newMessage != message)
    {
        m_rMessageHandler.printMessage(NfcNotify::toString(newMessage));
    }
    message = newMessage;
}

void Nfc_implementation::setTagOffline()
{
    m_rMfrc522.tagHalt();
    m_rMfrc522.tagLogoff();
}

bool Nfc_implementation::setTagOnline()
{
    bool status{false};
    // Try reading card
    status = m_rMfrc522.isCardPresent();
    m_pConcreteTag = &NfcTag_factory::getInstance(m_rMfrc522);
    return status;
}