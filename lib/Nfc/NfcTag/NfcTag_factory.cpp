#include "NfcTag_factory.h"

NfcTag_factory::~NfcTag_factory()
{
    removeInstance();
}

NfcTag_interface *NfcTag_factory::getInstance(MFRC522_interface &pMfrc522)
{
    removeInstance();
    MFRC522_interface::eTagType tagType = pMfrc522.getTagType();
    switch (tagType)
    {
    case MFRC522_interface::PICC_TYPE_MIFARE_MINI:
        m_instance = new NfcTag_MifareMini1k4k(pMfrc522);
        break;
    case MFRC522_interface::PICC_TYPE_MIFARE_1K:
        m_instance = new NfcTag_MifareMini1k4k(pMfrc522);
        break;
    case MFRC522_interface::PICC_TYPE_MIFARE_4K:
        m_instance = new NfcTag_MifareMini1k4k(pMfrc522);
        break;
    case MFRC522_interface::PICC_TYPE_MIFARE_UL:
        m_instance = new NfcTag_MifareUltralight(pMfrc522);
        break;
    default:
        m_instance = nullptr;
    }
    return m_instance;
};

void NfcTag_factory::removeInstance()
{
    delete m_instance;
    m_instance = nullptr;
}