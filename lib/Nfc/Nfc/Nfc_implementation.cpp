#include "Arduino_config.h"
#include "Nfc_implementation.h"

void Nfc_implementation::initNfc()
{
    m_rMessageHandler.printMessage(Message::TAGREADERINIT);
    m_rMfrc522.init(); // Init MFRC522
}

Message::eMessageContent Nfc_implementation::getTagPresence()
{
    Message::eMessageContent returnValue{Message::NOTAG};
    if (m_rMfrc522.isTagPresent())
    {
        if (m_wasTagPresent)
        {
            returnValue = Message::ACTIVETAG;
        }
        else
        {
            returnValue = Message::UNKNOWNTAG; // New tag, assume it is unknown
            m_wasTagPresent = true;
        }
    }
    else
    {
        if  (m_wasTagPresent)
        {
            returnValue = Message::REMOVEDTAG;
        }
        m_wasTagPresent = false;
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
    printNotification(status, Message::TAGWRITEOK, Message::TAGERRORWRITE);
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
    printNotification(status, Message::TAGREADOK, Message::TAGERRORREAD);
    return status;
}

void Nfc_implementation::printNotification(bool status, Message::eMessageContent successMessage, Message::eMessageContent failureMessage)
{
    Message::eMessageContent message{failureMessage};
    if (status)
    {
        message = successMessage;
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
    // Try to set card active, obtain Id and type, then fetch the right access method
    if(!m_rMfrc522.setTagActive())
    {
        m_rMessageHandler.printMessage(Message::NOTAG);
        return false;
    }
    if (!m_rMfrc522.getTagUid())
    {
        m_rMessageHandler.printMessage(Message::TAGERRORREAD);
        return false;
    }
    m_pConcreteTag = m_NfcTagFactory.getInstance(m_rMfrc522);
    if (m_pConcreteTag == nullptr)
    {
        m_rMessageHandler.printMessage(Message::TAGERRORTYPE);
        return false;
    }

    m_rMessageHandler.printMessage(Message::TAGONLINE);
    return true;
}