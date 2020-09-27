#ifndef NFCTAG_MIFAREULTRALIGHT_H
#define NFCTAG_MIFAREULTRALIGHT_H

#include "NfcTag_interface.h"
#include "MFRC522_interface.h"

class NfcTag_MifareUltralight : public NfcTag_interface
{
public:
    NfcTag_MifareUltralight(Nfc_interface *pMfrc522) : m_pMfrc522(pMfrc522) {}

public:
    bool readTag(byte blockAddress, byte *readResult) override;
    bool writeTag(byte blockAddress, byte *dataToWrite) override;
    eMFRC522Notify getNfcNotification() override { return m_eNotification; };

private:
    bool authenticateTag() override;
    void checkAndRectifyBlockAddress(byte &blockAddress) override;

private:
    Nfc_interface *m_pMfrc522{nullptr};
    eMFRC522Notify m_eNotification{noMessage};
    static const byte MIFARE_UL_BLOCK_SIZE{4};
    static const byte ULTRALIGHTSTARTPAGE{4};
    static const byte ULTRALIGHTSTOPPAGE{11};
};

#endif // NFCTAG_MIFAREULTRALIGHT_H