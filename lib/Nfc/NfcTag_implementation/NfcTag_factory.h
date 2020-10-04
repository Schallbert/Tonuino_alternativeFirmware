#ifndef NFCTAG_FACTORY_H
#define NFCTAG_FACTORY_H

#include "../NfcTag_interface/NfcTag_interface.h"
#include "NfcTag_MifareMini1k4k.h"
#include "NfcTag_MifareUltralight.h"

class NfcTag_factory
{
private:
    NfcTag_factory(){}; //cannot be instantiated from externally
public:
    // Returns a de-facto singleton of correct tag-Type
    static NfcTag_interface *getInstance(MFRC522_interface *pMfrc522)
    {
        MFRC522_interface::eTagType tagType = pMfrc522->getTagType();
        switch (tagType)
        {
        case MFRC522_interface::PICC_TYPE_MIFARE_MINI:
            return new NfcTag_MifareMini1k4k(pMfrc522);
            break;
        case MFRC522_interface::PICC_TYPE_MIFARE_1K:
            return new NfcTag_MifareMini1k4k(pMfrc522);
            break;
        case MFRC522_interface::PICC_TYPE_MIFARE_4K:
            return new NfcTag_MifareMini1k4k(pMfrc522);
            break;
        case MFRC522_interface::PICC_TYPE_MIFARE_UL:
            return new NfcTag_MifareUltralight(pMfrc522);
            break;
        }
        return nullptr;
    };
};

#endif //NFCTAG_FACTORY_H