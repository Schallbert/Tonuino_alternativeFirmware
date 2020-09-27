#ifndef NFCTAG_MIFAREMINI1K4K_H
#define NFCTAG_MIFAREMINI1K4K_H

#include "NfcTag_interface.h"
#include "MFRC522_interface.h"

class NfcTag_MifareMini1k4k : public NfcTag_interface
{
    public:
    NfcTag_MifareMini1k4k(Nfc_interface *pMfrc522) : m_pMfrc522(pMfrc522){}

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
    static const byte SECTORSTRAILERBLOCKMINI1K4K{3};
    byte m_ui8SectorMini1k4k = {0};
    byte m_ui8TrailerBlockMini1k4k{3};
};

#endif // NFCTAG_MIFAREMINI1K4K_H