#include "NfcTag_factory.h"

NfcTag_interface *NfcTag_factory::m_pNfcTag;

NfcTag_interface *NfcTag_factory::getInstance(MFRC522_interface::eTagType tagType, MFRC522_interface *pMfrc522)
    {
        NfcTag_factory::removeInstance(); // remove "old" instance if necessary
        switch (tagType)
        {
        case MFRC522_interface::PICC_TYPE_MIFARE_MINI:
            m_pNfcTag = new NfcTag_MifareMini1k4k(pMfrc522);
            break;
        case MFRC522_interface::PICC_TYPE_MIFARE_1K:
            m_pNfcTag = new NfcTag_MifareMini1k4k(pMfrc522);
            break;
        case MFRC522_interface::PICC_TYPE_MIFARE_4K:
            m_pNfcTag = new NfcTag_MifareMini1k4k(pMfrc522);
            break;
        case MFRC522_interface::PICC_TYPE_MIFARE_UL:
            m_pNfcTag = new NfcTag_MifareUltralight(pMfrc522);
            break;
        default:
            m_pNfcTag = nullptr;
        }
        return m_pNfcTag;
    }

    void NfcTag_factory::removeInstance()
    {
        if (m_pNfcTag)
        {
            delete m_pNfcTag;
        }
    }


   