#include "Nfc.h"

Nfc_interface::eTagState Nfc::getTagPresence()
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
            if (setCardOnline())
            {
                return NEW_TAG;
            }
        }
    }
    return NO_TAG;
}

const char *Nfc::stringFromNfcNotify(eNfcNotify value)
{
#if DEBUGSERIAL
    static const char *NOTIFY_STRING[] = {
        "no Message",
        "Tag online",
        "Tag offline",
        "Tag write Error",
        "Tag read Error",
        "Tag type Error: unknown or not implemented",
        "Tag set online failed",
        "Warning: request out of Memory Range"};

    return NOTIFY_STRING[value];
#endif
    return "";
}

const char *Nfc::getNfcNotification()
{
    return stringFromNfcNotify(m_eNotification);
}

void Nfc::initNfc()
{
    m_pMfrc522->init(); // Init MFRC522
}

bool Nfc::writeTag(byte blockAddr, byte *dataToWrite)
{
    NfcTag_interface *pNfcTag = NfcTag_factory::getInstance(m_tagType);
    if(!pNfcTag)
    {
        // returned nullptr, not implemented
        m_eNotification = errorTagTypeNotImplemented;
    }
    if(!pNfcTag->writeTag(blockAddr, dataToWrite))
    {
        m_eNotification = errorTagWrite;
        return false;
    }
    return true;
}

bool Nfc::readTag(byte blockAddr, byte *readResult)
{
   NfcTag_interface *pNfcTag = NfcTag_factory::getInstance(m_tagType);
    if(!pNfcTag)
    {
        // returned nullptr, not implemented
        m_eNotification = errorTagTypeNotImplemented;
    }
    if(!pNfcTag->readTag(blockAddr, readResult))
    {
        m_eNotification = errorTagRead;
        return false;
    }
    return true;
}

void Nfc::setCardOffline()
{
    m_pMfrc522->tagHalt();
    m_pMfrc522->tagLogoff();
    m_eNotification = tagOffline;
}

bool Nfc::setCardOnline()
{
    // Try reading card
    if (!m_pMfrc522->isCardPresent())
    {
        setCardOffline();
        m_eNotification = errorTagSetOnlineFailed;
        return false;
    }
    if (!getTagType())
    {
        return false;
    }
    m_eNotification = tagOnline;
    return true;
}

bool Nfc::getTagType()
{
    m_tagType = m_pMfrc522->getTagType();
    if(m_tagType != MFRC522_interface::PICC_TYPE_UNKNOWN)
    {
        return true;
    }
    m_eNotification = errorTagRead;
    return false;
}