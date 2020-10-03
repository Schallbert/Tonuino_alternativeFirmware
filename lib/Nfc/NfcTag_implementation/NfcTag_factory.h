#ifndef NFCTAG_FACTORY_H
#define NFCTAG_FACTORY_H

#include "../NfcTag_interface/NfcTag_interface.h"
#include "NfcTag_MifareMini1k4k.h"
#include "NfcTag_MifareUltralight.h"

class NfcTag_factory
{
public:
    // Returns a de-facto singleton of correct tag-Type
    static NfcTag_interface *getInstance(MFRC522_interface *pMfrc522);
    static void removeInstance();
  
private:
    static NfcTag_interface *m_pNfcTag;
};

#endif //NFCTAG_FACTORY_H