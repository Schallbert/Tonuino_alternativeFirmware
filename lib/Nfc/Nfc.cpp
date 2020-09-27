#include "Nfc.h"

Nfc_interface::eTagState Nfc::getTagPresence()
{
    if (m_pMfrc522->)
    {
        // A card is present!
        if (!m_pMfrc522.PICC_IsNewCardPresent())
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

const char *Nfc::stringFromNfcNotify(eMFRC522Notify value)
{
#if DEBUGSERIAL
    static const char *NOTIFY_STRING[] = {
        "no Message",
        "Tag online",
        "Tag offline",
        "Tag write Error",
        "Tag read Error",
        "Tag set online failed",
        "Tag authentication failed",
        "request out of Memory Range"};

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
    m_pMfrc522.PCD_Init(); // Init MFRC522
}

bool Nfc::writeTag(byte blockAddr, byte *dataToWrite)
{
    NfcTag_interface *pNfcTag = NfcTag_factory::getInstance();
    return pNfcTag->writeTag(blockAddr, dataToWrite);
}

bool Nfc::readTag(byte blockAddr, byte *readResult)
{
    NfcTag_interface *pNfcTag = NfcTag_factory::getInstance();
    return pNfcTag->readTag(blockAddr, dataToWrite);
}

void Nfc::setCardOffline()
{
    m_pMfrc522.PICC_HaltA();
    m_pMfrc522.PCD_StopCrypto1();
    m_eNotification = tagOffline;
}

bool Nfc::setCardOnline()
{
    // Try reading card
    if (m_pMfrc522.PICC_ReadCardSerial() != MFRC522::STATUS_OK)
    {
        m_eNotification = errorTagSetOnlineFailed;
        setCardOffline();
        return false;
    }
    if (!getTagType())
    {
        m_eNotification = errorTagAuthenticateFailed;
        return false;
    }
    m_eNotification = tagOnline;
    return true;
}

bool Nfc::getTagType()
{
    m_tagType = m_pMfrc522.PICC_GetType(m_pMfrc522.uid.sak);
    return (m_tagType != MFRC522::PICC_TYPE_UNKNOWN);
}