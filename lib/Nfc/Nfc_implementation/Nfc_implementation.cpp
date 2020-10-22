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

    m_eNotification = noMessage; // reset notification
    return returnValue;
}

const char *Nfc_implementation::stringFromNfcNotify(eNfcNotify value)
{
#if DEBUGSERIAL
    static const char *NOTIFY_STRING[number_of_notifications] = {
        "no Message",
        "Tag Write Success",
        "Tag Read Success",
        "Tag write Error",
        "Tag read Error",
        "Tag type unknown/not implemented",
        "Warning: request out of Memory Range"};

    return NOTIFY_STRING[value];
#endif
    return "";
}

const char *Nfc_implementation::getNfcNotification()
{
    return stringFromNfcNotify(m_eNotification);
}

void Nfc_implementation::initNfc()
{
    m_pMfrc522->init(); // Init MFRC522
}

bool Nfc_implementation::writeTag(byte blockAddress, byte *dataToWrite)
{
    bool status{false};
    if(setTagOnline())
    {
        status = m_pConcreteTag->writeTag(blockAddress, dataToWrite);
    }
    setTagOffline();
    setNotification(status, tagWriteSuccess, tagWriteError);
    return status;
}

bool Nfc_implementation::readTag(byte blockAddress, byte *readResult)
{
    bool status{false};
    if(setTagOnline())
    {
        status = m_pConcreteTag->readTag(blockAddress, readResult);
    }
    setTagOffline();
    setNotification(status, tagReadSuccess, tagReadError);
    return status;
}

void Nfc_implementation::setNotification(bool status, eNfcNotify successMessage, eNfcNotify failureMessage)
{
    if(status)
    {
         m_eNotification = successMessage;
    }
    else
    {
        m_eNotification = failureMessage;
    } 
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
    if(m_pConcreteTag)
    {
        delete m_pConcreteTag; // make sure to delete earlier instances (mem leak)
    }
    m_pConcreteTag = NfcTag_factory::getInstance(m_pMfrc522);
    if (m_pConcreteTag)
    {
        status = true; // returned non-null ptr, tag type implemented
    }
    setNotification(status, noMessage, tagTypeNotImplementedError);
    return status;
}