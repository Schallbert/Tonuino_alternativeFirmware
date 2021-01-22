#include "Arduino_config.h"
#include "Nfc_implementation.h"

void Nfc_implementation::initNfc()
{
    m_rMfrc522.init(); // Init MFRC522
    Message init{Message(Message::READERINIT)};
    m_rMessageHandler.printMessage(init);
}

Message::eMessageContent Nfc_implementation::getTagPresence()
{
    Message::eMessageContent returnValue{Message::NOTAG};
    if (m_rMfrc522.isCardPresent())
    {
        // A card is present!
        if (!m_rMfrc522.isNewCardPresent())
        {
            returnValue = Message::ACTIVETAG;
        }
        else
        {
            // New card detected: runs once as new card is automatically set to ActiveCard
            if (setTagOnline())
            {
                returnValue = Message::UNKNOWNTAG; // assume tag is unknown
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
    printNotification(status, Message::WRITEOK, Message::ERRORWRITE);
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
    printNotification(status, Message::READOK, Message::ERRORREAD);
    return status;
}

void Nfc_implementation::printNotification(bool status, Message::eMessageContent successMessage, Message::eMessageContent failureMessage)
{
    Message message{Message(Message::ERRORTYPE)};
    if (status)
    {
        message.setContents(successMessage);
    }
    else
    {
        message.setContents(failureMessage);
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
    printNotification(status, Message::ONLINE, Message::ERRORTYPE);
    return status;
}