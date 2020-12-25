#ifndef NFCTAG_FACTORY_H
#define NFCTAG_FACTORY_H

#include "../NfcTag/NfcTag_interface.h"
#include "NfcTag_MifareMini1k4k.h"
#include "NfcTag_MifareUltralight.h"

class NfcTag_factory
{
private:
    NfcTag_factory(){}; //cannot be instantiated from externally
public:
    // Returns a de-facto singleton of correct tag-Type
    static NfcTag_interface &getInstance(MFRC522_interface &rMfrc522)
    {
        static NfcTag_MifareUltralight nfcUltralight{NfcTag_MifareUltralight(rMfrc522)};
        static NfcTag_MifareMini1k4k nfcNormal{NfcTag_MifareMini1k4k(rMfrc522)};
        MFRC522_interface::eTagType tagType = rMfrc522.getTagType();
        switch (tagType)
        {
        case MFRC522_interface::PICC_TYPE_MIFARE_MINI:
            return nfcNormal;
            break;
        case MFRC522_interface::PICC_TYPE_MIFARE_1K:
            return nfcNormal;
            break;
        case MFRC522_interface::PICC_TYPE_MIFARE_4K:
            return nfcNormal;
            break;
        case MFRC522_interface::PICC_TYPE_MIFARE_UL:
            return nfcUltralight;
            break;
        default:
            return nfcNormal;
        }
    };
};

#endif //NFCTAG_FACTORY_H