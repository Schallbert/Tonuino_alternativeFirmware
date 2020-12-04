#ifndef NFCTAG_MIFAREULTRALIGHT_H
#define NFCTAG_MIFAREULTRALIGHT_H

#include "../Nfc/NfcTag/NfcTag_interface.h"
#include "../Nfc/MFRC522/MFRC522_interface.h"

class NfcTag_MifareUltralight : public NfcTag_interface
{
public:
    NfcTag_MifareUltralight(MFRC522_interface *pMfrc522) : m_pMfrc522(pMfrc522) {}

public:
    bool readTag(byte blockAddress, byte *readResult) override;
    bool writeTag(byte blockAddress, byte *dataToWrite) override;

private:
    void checkAndRectifyBlockAddress(byte &blockAddress) override;

public:
    static const byte MIFARE_UL_BLOCK_SIZE{4};
    static const byte ULTRALIGHTSTARTPAGE{4};
    static const byte ULTRALIGHTSTOPPAGE{11};

private:
    MFRC522_interface *m_pMfrc522{nullptr};
};

#endif // NFCTAG_MIFAREULTRALIGHT_H