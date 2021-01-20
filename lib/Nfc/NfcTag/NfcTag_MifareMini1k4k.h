#ifndef NFCTAG_MIFAREMINI1K4K_H
#define NFCTAG_MIFAREMINI1K4K_H

#include "../Nfc/NfcTag/NfcTag_interface.h"
#include "../Nfc/MFRC522/MFRC522_interface.h"

class NfcTag_MifareMini1k4k : public NfcTag_interface
{
public:
    explicit NfcTag_MifareMini1k4k(MFRC522_interface &rMfrc522) : m_rMfrc522(rMfrc522) {}

public:
    bool readTag(byte blockAddress, byte *readResult) override;
    bool writeTag(byte blockAddress, byte *dataToWrite) override;

private:
    void checkAndRectifyBlockAddress(byte &blockAddress) override;

private:
    MFRC522_interface &m_rMfrc522;
    static const byte SECTORSTRAILERBLOCKMINI1K4K{3};
    byte m_ui8SectorMini1k4k = {0};
    byte m_ui8TrailerBlockMini1k4k{3};
};

#endif // NFCTAG_MIFAREMINI1K4K_H