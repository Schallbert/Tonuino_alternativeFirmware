#ifndef NFCTAG_MIFAREULTRALIGHT_H
#define NFCTAG_MIFAREULTRALIGHT_H

#include "../Nfc/NfcTag/NfcTag_interface.h"
#include "../Nfc/MFRC522/MFRC522_interface.h"

constexpr byte MIFARE_UL_BLOCK_SIZE{4};
constexpr byte ULTRALIGHTSTARTPAGE{4};
constexpr byte ULTRALIGHTSTOPPAGE{11};

class NfcTag_MifareUltralight : public NfcTag_interface
{
public:
    explicit NfcTag_MifareUltralight(MFRC522_interface &rMfrc522) : m_rMfrc522(rMfrc522) {}
    ~NfcTag_MifareUltralight() = default;
    NfcTag_MifareUltralight(const NfcTag_MifareUltralight &cpy) = delete;

public:
    bool readTag(byte blockAddress, byte *readResult) override;
    bool writeTag(byte blockAddress, byte *dataToWrite) override;

private:
    void checkAndRectifyBlockAddress(byte &blockAddress) override;

private:
    MFRC522_interface &m_rMfrc522;
};

#endif // NFCTAG_MIFAREULTRALIGHT_H