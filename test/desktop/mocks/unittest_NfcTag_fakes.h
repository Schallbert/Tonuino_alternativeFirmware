#ifndef UNITTEST_NFCTAG_FAKES_H
#define UNITTEST_NFCTAG_FAKES_H

#include <MFRC522_interface.h>
#include <NfcTag.h>

// Fake buffer data for NFC tag read
static const byte fakeBufferData[16]{
    (byte)(NfcTag::cui32MagicCookie >> 24),          // 0
    (byte)((NfcTag::cui32MagicCookie >> 16) & 0xFF), // 1
    (byte)((NfcTag::cui32MagicCookie >> 8) & 0xFF),  // 2
    (byte)(NfcTag::cui32MagicCookie & 0xFF),         // 3
    (byte)1,                                         // 4 FolderId
    (byte)Folder::LULLABYE,                          // 5 ePlayMode
    (byte)5,                                         // 6 TrackCount
    0, 0, 0, 0, 0, 0, 0, 0, 0};

// FAKES

class Fake_MFRC522 : public MFRC522_interface
{
public:
    virtual ~Fake_MFRC522() {}; // MUST BE DEFINED; ELSE VTABLE INCLUDE ERRORS
    void initReader() override;
    bool isCardPresent() override;
    bool isNewCardPresent() override;
    bool writeCard(byte blockAddr, byte *dataToWrite) override;
    bool readCard(byte blockAddr, byte *readResult) override;
};
#endif //UNITTEST_NFCTAG_FAKES_H