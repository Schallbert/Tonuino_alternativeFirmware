#ifndef NFCTAG_FACTORY_H
#define NFCTAG_FACTORY_H

#include "../NfcTag/NfcTag_interface/NfcTag_interface.h"
#include "../NfcTag/NfcTag_implementation/NfcTag_MifareMini1k4k.h"
#include "../NfcTag/NfcTag_implementation/NfcTag_MifareUltralight.h"

class NfcTag_factory
{

public:
    // Returns a de-facto singleton of correct tag-Type
    static NfcTag_interface *getInstance(MFRC522_interface::eTagType tagType, MFRC522_interface *pMfrc522)
    {
        removeInstance(); // remove "old" instance if necessary
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

    static void removeInstance()
    {
        if (m_pNfcTag)
        {
            delete m_pNfcTag;
        }
    }

private:
    static NfcTag_interface *m_pNfcTag;
};

#endif //NFCTAG_FACTORY_H