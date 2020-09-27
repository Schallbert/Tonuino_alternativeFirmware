#ifndef NFCTAG_FACTORY_H
#define NFCTAG_FACTORY_H

#include "MFRC522.h"
#include "NfcTag_interface.h"

class NfcTag_factory
{

public:
    static NfcTag_interface *getInstance(MFRC522::PICC_Type tagType)
    {
        switch (tagType)
        {
        case MFRC522::PICC_TYPE_MIFARE_MINI:
            m_pObject = new NfcTag_implementation_MifareMini1k4k;
            break;
        case MFRC522::PICC_TYPE_MIFARE_1K:
            m_pObject = new NfcTag_implementation_MifareMini1k4k;
            break;
        case MFRC522::PICC_TYPE_MIFARE_4K:
            m_pObject = new NfcTag_implementation_MifareMini1k4k;
            break;
        case MFRC522::PICC_TYPE_MIFARE_UL:
            m_pObject = new NfcTag_implementation_MifareUltraLight;
            break;
        default:
            m_pObject = nullptr;
        }
        return m_pObject;
    }

    static void removeInstance()
    {
        if (m_pObject)
        {
            delete m_pObject;
        }
    }

private:
    static NfcTag_interface *m_pObject;
};

#endif //NFCTAG_FACTORY_H