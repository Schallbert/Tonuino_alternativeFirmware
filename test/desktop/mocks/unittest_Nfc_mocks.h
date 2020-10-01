#ifndef UNITTEST_NFCTAG_MOCKS_H
#define UNITTEST_NFCTAG_MOCKS_H

#include "gmock/gmock.h"
#include "../Arduino/Arduino_interface/Arduino_types.h"
#include "../Nfc/Nfc_interface/Nfc_interface.h"
#include "../Nfc/NfcTag_interface/NfcTag_interface.h"


// FAKES
// Fake buffer data for NFC tag read
static const byte fakeBufferData[16]{
    (byte)(NfcControl::cui32MagicCookie >> 24),          // 0
    (byte)((NfcControl::cui32MagicCookie >> 16) & 0xFF), // 1
    (byte)((NfcControl::cui32MagicCookie >> 8) & 0xFF),  // 2
    (byte)(NfcControl::cui32MagicCookie & 0xFF),         // 3
    (byte)1,                                         // 4 FolderId
    (byte)Folder::LULLABYE,                          // 5 ePlayMode
    (byte)5,                                         // 6 TrackCount
    0, 0, 0, 0, 0, 0, 0, 0, 0};

class Fake_Nfc : public Nfc_interface
{
public:
    virtual ~Fake_Nfc() {}; // MUST BE DEFINED; ELSE VTABLE INCLUDE ERRORS
    void initNfc() override;
    Nfc_interface::eTagState getTagPresence() override;
    // returns true, simulating successful write
    bool writeTag(byte blockAddress, byte *dataToWrite) override;
    // copies fakeBufferData to "readResult", simulating read from NFC tag
    bool readTag(byte blockAddress, byte *readResult) override;
    const char *getNfcNotification()  override;
};


// MOCKS
class Mock_Nfc : public Nfc_interface
{
public:
    // Method:  output name   input   overrides functionality of interface
    // IF THE METHOD IS INPUTS VOID; JUST WRITE () NEVER WRITE (void) !!!
    MOCK_METHOD(void, initNfc, (), (override));
    MOCK_METHOD(Nfc_interface::eTagState, getTagPresence, (), (override));
    MOCK_METHOD(bool, writeTag, (byte blockAddress, byte *dataToWrite), (override));
    MOCK_METHOD(bool, readTag, (byte blockAddress, byte *readResult), (override));
    MOCK_METHOD(const char *, getNfcNotification, (), (override));

    void DelegateToFake()
    {
        ON_CALL(*this, readTag).WillByDefault([this](byte blockAddress, byte *readResult) 
        {
            return m_FakeRead.readTag(blockAddress, readResult);
        });
    }

private:
    Fake_Nfc m_FakeRead {};
};

class Mock_NfcTag : public NfcTag_interface
{
     public:
    MOCK_METHOD(bool, readTag, (byte blockAddress, byte *readResult), (override));
    // Write data to block for for implemented NFC types
    MOCK_METHOD(bool, writeTag, (byte blockAddress, byte *dataToWrite), (override));

//private:
    //MOCK_METHOD(void, checkAndRectifyBlockAddress(byte &blockAddress));
};

// MATCHERS
MATCHER_P2(arrayByteCompare, bytes, size, "Compares array bites and throws errors for each byte that does not match.")
{
    for (int i = 0; i < size; ++i)
    {
        if (arg[i] != bytes[i])
        {
            EXPECT_EQ("", "Error: " + std::to_string(i) + ". element not matching:");
            EXPECT_EQ(arg[i], bytes[i]);
        }
    }
    return true;
}

#endif // UNITTEST_NFCTAG_MOCKS_H