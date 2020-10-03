#include "Nfc_implementation.h"

Nfc_interface::eTagState Nfc_implementation::getTagPresence()
{
    
    if (m_pMfrc522->isCardPresent())
    {
        // A card is present!
        if (!m_pMfrc522->isNewCardPresent())
        {
            return ACTIVE_KNOWN_TAG;
        }
        else
        {
            // New card detected: runs once as new card is automatically set to ActiveCard
            if (setTagOnline())
            {
                return NEW_UNKNOWN_TAG; // assume tag is unknown
            }
        }
    }

    if (m_eNotification == tagWriteError ||
        m_eNotification == tagReadError ||
        m_eNotification == tagTypeNotImplementedError ||
        m_eNotification == tagSetOnlineFailed)
    {
        return ERROR;
    }

    return NO_TAG;
}

const char *Nfc_implementation::stringFromNfcNotify(eNfcNotify value)
{
#if DEBUGSERIAL
    static const char *NOTIFY_STRING[] = {
        "no Message",
        "Tag Write Success",
        "Tag Read Success",
        "Tag write Error",
        "Tag read Error",
        "Tag type unknown/not implemented",
        "Tag set online failed",
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

bool Nfc_implementation::writeTag(byte blockAddr, byte *dataToWrite)
{
    bool status{false};
    NfcTag_interface *pNfcTag = NfcTag_factory::getInstance(m_tagType, m_pMfrc522);
    if (!pNfcTag)
    {
        setNotification(false, noMessage, tagTypeNotImplementedError);
        return status; // returned nullptr, tag type not implemented
    }
    status = pNfcTag->writeTag(blockAddr, dataToWrite);
    setTagOffline();
    setNotification(status, tagWriteSuccess, tagWriteError);
    return status;
}

bool Nfc_implementation::readTag(byte blockAddr, byte *readResult)
{
    bool status{false};
    NfcTag_interface *pNfcTag = NfcTag_factory::getInstance(m_tagType, m_pMfrc522);
    if (!pNfcTag)
    {
        setNotification(false, noMessage, tagTypeNotImplementedError);
        return status; // returned nullptr, tag type not implemented
    }
    status = pNfcTag->readTag(blockAddr, readResult);
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
    status &= getTagType();
    return status;
}

bool Nfc_implementation::getTagType()
{
    bool status{true};
    m_tagType = m_pMfrc522->getTagType();
    if (m_tagType == (MFRC522_interface::PICC_TYPE_UNKNOWN ||
                      MFRC522_interface::PICC_TYPE_NOT_COMPLETE))
    {
        status = false;
    }
    setNotification(status, noMessage, tagTypeNotImplementedError);
    return status;
}