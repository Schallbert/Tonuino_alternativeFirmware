#ifndef NFCTAG_FACTORY_H
#define NFCTAG_FACTORY_H

#include "NfcTag_interface.h"
#include "NfcTag_MifareMini1k4k.h"
#include "NfcTag_MifareUltralight.h"

class NfcTag_factory
{

public:
    static NfcTag_interface *getInstance(MFRC522::PICC_Type tagType)
    {
        switch (tagType)
        {
        case MFRC522::PICC_TYPE_MIFARE_MINI:
            m_pNfcTag = new NfcTag_MifareMini1k4k;
            break;
        case MFRC522::PICC_TYPE_MIFARE_1K:
            m_pNfcTag = new NfcTag_MifareMini1k4k;
            break;
        case MFRC522::PICC_TYPE_MIFARE_4K:
            m_pNfcTag = new NfcTag_MifareMini1k4k;
            break;
        case MFRC522::PICC_TYPE_MIFARE_UL:
            m_pNfcTag = new NfcTag_MifareUltraLight;
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