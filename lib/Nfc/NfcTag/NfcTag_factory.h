#ifndef NFCTAG_FACTORY_H
#define NFCTAG_FACTORY_H

#include "../NfcTag/NfcTag_interface.h"
#include "NfcTag_MifareMini1k4k.h"
#include "NfcTag_MifareUltralight.h"

class NfcTag_factory
{
public:
    NfcTag_factory() = default;
    ~NfcTag_factory();
    NfcTag_factory(const NfcTag_factory &cpy) = delete;

public:
    // Returns a de-facto singleton of correct tag-Type
    NfcTag_interface *getInstance(MFRC522_interface &pMfrc522);

private:
    void removeInstance();

private:
    NfcTag_interface *m_instance{nullptr};
};

#endif //NFCTAG_FACTORY_H