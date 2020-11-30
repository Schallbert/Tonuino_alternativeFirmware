#include "Nfc_implementation.h"

Nfc_interface::eTagState Nfc_implementation::getTagPresence()
{
    Nfc_interface::eTagState returnValue{NO_TAG};
    if (m_pMfrc522->isCardPresent())
    {
        // A card is present!
        if (!m_pMfrc522->isNewCardPresent())
        {
            returnValue = ACTIVE_KNOWN_TAG;
        }
        else
        {
            // New card detected: runs once as new card is automatically set to ActiveCard
            if (setTagOnline())
            {
                returnValue = NEW_UNKNOWN_TAG; // assume tag is unknown
            }
        }
    }
    return returnValue;
}

void Nfc_implementation::initNfc()
{
    m_pMfrc522->init(); // Init MFRC522
}

bool Nfc_implementation::writeTag(byte blockAddress, byte *dataToWrite)
{
    bool status{false};
    if (setTagOnline())
    {
        status = m_pConcreteTag->writeTag(blockAddress, dataToWrite);
    }
    setTagOffline();
    setNotification(status, NfcNotify::tagWriteSuccess, NfcNotify::tagWriteError);
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
    setNotification(status, NfcNotify::tagReadSuccess, NfcNotify::tagReadError);
    return status;
}

void Nfc_implementation::setNotification(bool status, NfcNotify::eNfcNotify successMessage, NfcNotify::eNfcNotify failureMessage)
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
        m_pMessageHandler->printMessage(NfcNotify::toString(newMessage));
    }
    message = newMessage;
}

void Nfc_implementation::setTagOffline()
{
    m_pMfrc522->tagHalt();
    m_pMfrc522->tagLogoff();
}

bool Nfc_implementation::setTagOnline()
{
    bool status{false};
    // Try reading card
    status = m_pMfrc522->isCardPresent();
    status &= getTag();
    return status;
}

bool Nfc_implementation::getTag()
{
    bool status{false};
    if (m_pConcreteTag)
    {
        delete m_pConcreteTag; // make sure to delete earlier instances (mem leak)
    }
    m_pConcreteTag = NfcTag_factory::getInstance(m_pMfrc522);
    if (m_pConcreteTag)
    {
        status = true; // returned non-null ptr, tag type implemented
    }
    setNotification(status, NfcNotify::noMessage, NfcNotify::tagTypeNotImplementedError);
    return status;
}