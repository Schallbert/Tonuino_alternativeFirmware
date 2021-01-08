#include "Arduino_config.h"
#include "Nfc_implementation.h"

void Nfc_implementation::initNfc()
{
    m_rMfrc522.init(); // Init MFRC522
    Message init{Message(Messages_interface::NFCREADER, Messages_interface::READERINIT)};
    m_rMessageHandler.printMessage(init);
}

NfcControl_interface::eTagState Nfc_implementation::getTagPresence()
{
    NfcControl_interface::eTagState returnValue{NfcControl_interface::NO_TAG};
    if (m_rMfrc522.isCardPresent())
    {
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

bool Nfc_implementation::writeTag(byte blockAddress, byte *dataToWrite)
{
    bool status{false};
    if (setTagOnline())
    {
        status = m_pConcreteTag->writeTag(blockAddress, dataToWrite);
    }
    setTagOffline();
    printNotification(status, Messages_interface::WRITEOK, Messages_interface::ERRORWRITE);
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
    printNotification(status, Messages_interface::READOK, Messages_interface::ERRORREAD);
    return status;
}

void Nfc_implementation::printNotification(bool status, Messages_interface::eMessageContent successMessage, Messages_interface::eMessageContent failureMessage)
{
    Message message{Message(Messages_interface::NFCREADER, Messages_interface::ERRORTYPE)};
    if (status)
    {
        message.m_contents = successMessage;
    }
    else
    {
        message.m_contents = failureMessage;
    }
    m_rMessageHandler.printMessage(message);
}

void Nfc_implementation::setTagOffline()
{
    m_rMfrc522.tagHalt();
    m_rMfrc522.tagLogoff();
}

bool Nfc_implementation::setTagOnline()
{
    bool status{true};
    // Try reading card
    status &= m_rMfrc522.isCardPresent();
    m_pConcreteTag = m_NfcTagFactory.getInstance(m_rMfrc522);
    status &= (m_pConcreteTag != nullptr); // Not implemented if factory cannot respond OK
    printNotification(status, Messages_interface::READOK, Messages_interface::ERRORTYPE);
    return status;
}