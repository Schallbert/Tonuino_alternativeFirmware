#ifndef NFCTAG_IMPLEMENTATION_MIFAREMINI1K4K_H
#define NFCTAG_IMPLEMENTATION_MIFAREMINI1K4K_H

#include "NfcTag_interface.h"
#include "MFRC522_interface.h"

class NfcTag_implementation_MifareMini1k4k : public NfcTag_interface
{
    public:
    NfcTag_implementation_MifareMini1k4k(MFRC522_interface *pMfrc522) : m_pMfrc522(pMfrc522){}

    public:
    bool readTag(byte blockAddr, byte *readResult) override;
    bool writeTag(byte blockAddr, byte *dataToWrite) override;

    private:
    bool authenticateCard() override;
    void checkAndRectifyBlockAddress(byte &blockAddress) override;

    private:
    MFRC522_interface *m_pMfrc522;
};

#endif // NFCTAG_IMPLEMENTATION_MIFAREMINI1K4K_H