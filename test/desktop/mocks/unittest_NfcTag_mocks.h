#ifndef UNITTEST_NFCTAG_MOCKS_H
#define UNITTEST_NFCTAG_MOCKS_H

#include <gmock/gmock.h>
#include <Arduino_types.h>
#include <MFRC522_interface.h>
#include <NfcControl.h>

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

class Fake_MFRC522 : public MFRC522_interface
{
public:
    virtual ~Fake_MFRC522() {}; // MUST BE DEFINED; ELSE VTABLE INCLUDE ERRORS
    void initReader() override;
    bool isCardPresent() override;
    bool isNewCardPresent() override;
    bool readTag(byte blockAddr, byte *readResult) override;
    bool writeTag(byte blockAddr, byte *dataToWrite) override;
    const char *getMFRC522Notification()  override;
};


// MOCKS
class Mock_MFRC522 : public MFRC522_interface
{
public:
    // Method:  output name   input   overrides functionality of interface
    // IF THE METHOD IS INPUTS VOID; JUST WRITE () NEVER WRITE (void) !!!
    MOCK_METHOD(void, initReader, (), (override));
    MOCK_METHOD(bool, isCardPresent, (), (override));
    MOCK_METHOD(bool, isNewCardPresent, (), (override));
    MOCK_METHOD(bool, writeTag, (byte blockAddr, byte *dataToWrite), (override));
    MOCK_METHOD(bool, readTag, (byte blockAddr, byte *readResult), (override));
    MOCK_METHOD(const char *, getMFRC522Notification, (), (override));

    void DelegateToFake()
    {
        ON_CALL(*this, readTag).WillByDefault([this](byte blockAddr, byte *readResult) 
        {
            return m_FakeRead.readTag(blockAddr, readResult);
        });
    }

private:
    Fake_MFRC522 m_FakeRead {};
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