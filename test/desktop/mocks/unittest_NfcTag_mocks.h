#include <Arduino.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <MFRC522_interface.h>
#include <NfcTag.h>

using ::testing::NiceMock;

static const byte expectedBufferData[16]{
    (byte)(NfcTag::cui32MagicCookie >> 24),          // 0
    (byte)((NfcTag::cui32MagicCookie >> 16) & 0xFF), // 1
    (byte)((NfcTag::cui32MagicCookie >> 8) & 0xFF),  // 2
    (byte)(NfcTag::cui32MagicCookie & 0xFF),         // 3
    (byte)1,                                         // 4 FolderId
    (byte)Folder::LULLABYE,                          // 5 ePlayMode
    (byte)5,                                         // 6 TrackCount
    0, 0, 0, 0, 0, 0, 0, 0, 0};

// MOCKS
class Mock_MFRC522 : public MFRC522_interface
{
public:
    // Method:  output name   input   overrides functionality of interface
    // IF THE METHOD IS INPUTS VOID; JUST WRITE () NEVER WRITE (void) !!!
    MOCK_METHOD(void, initReader, (), (override));
    MOCK_METHOD(bool, isCardPresent, (), (override));
    MOCK_METHOD(bool, isNewCardPresent, (), (override));
    MOCK_METHOD(bool, writeCard, (byte blockAddr, byte *dataToWrite), (override));
    MOCK_METHOD(bool, readCard, (byte blockAddr, byte *readResult), (override));
};

// FAKES

class Fake_MFRC522 : public MFRC522_interface
{
public:
    bool readCard(byte blockAddr, byte *readResult) override final
    {
        // copies expectedBufferData to "readResult", simulating read from NFC tag
        for (int i = 0; i < MFRC522_interface::NFCTAG_MEMORY_TO_OCCUPY; ++i) // 7-15: Empty
        {
            readResult[i] = expectedBufferData[i];
        }
        return true;
    }
};

// TEST FIXTURE
class NfcTagReadWrite : public ::testing::Test
{
protected:
    // Arrange
    virtual void SetUp()
    {
        m_pMfrc = new NiceMock<Mock_MFRC522>;
        m_pEeprom = new Mock_Eeprom;
        m_pNfc = new NfcTag(m_pMfrc);
        m_pTestFolder = new Folder(expectedBufferData[4],
                                (Folder::ePlayMode)expectedBufferData[5],
                                expectedBufferData[6]);
        m_pTestFolder->setup_dependencies(m_pEeprom, 0);
    }

    virtual void TearDown()
    {
        delete m_pMfrc;
        delete m_pNfc;
        delete m_pEeprom;
        delete m_pTestFolder;
    }

protected:
    NiceMock<Mock_MFRC522> *m_pMfrc;
    NfcTag *m_pNfc;
    Mock_Eeprom *m_pEeprom;
    Folder *m_pTestFolder;
};
