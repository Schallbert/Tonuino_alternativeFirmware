#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <NfcTag.h>
#include "mocks/unittest_Folder_mocks.h"
#include "mocks/unittest_NfcTag_mocks.h"

using ::testing::Return;
//using ::testing::AtLeast;
using ::testing::_;
using ::testing::ElementsAre;
using ::testing::NiceMock;

// TEST FIXTURE
class NfcTag_Write : public ::testing::Test
{
protected:
    // Arrange
    virtual void SetUp()
    {
        mfrc = new NiceMock<Mock_MFRC522>;
        eeprom = new Mock_Eeprom;
        nfc = new NfcTag(mfrc);
        testFolder = new Folder(1, Folder::ALBUM, 5);
        testFolder->setup_dependencies(eeprom, 0);
        
        expectedBufferData = new byte[16]{
            (byte)(NfcTag::cui32MagicCookie >> 24),          // 0
            (byte)((NfcTag::cui32MagicCookie >> 16) & 0xFF), // 1
            (byte)((NfcTag::cui32MagicCookie >> 8) & 0xFF),  // 2
            (byte)(NfcTag::cui32MagicCookie & 0xFF),         // 3
            (byte)testFolder->get_folder_id(),               // 4
            (byte)testFolder->get_play_mode(),               // 5
            (byte)testFolder->get_track_count(),             // 6
            0, 0, 0, 0, 0, 0, 0, 0, 0};
    }

    virtual void TearDown()
    {
        delete mfrc;
        delete nfc;
        delete eeprom;
        delete testFolder;
        delete[] expectedBufferData; 
    }

protected:
    NiceMock<Mock_MFRC522>* mfrc;
    NfcTag* nfc;
    Mock_Eeprom* eeprom;
    Folder* testFolder;
    byte* expectedBufferData;
};


// TESTS
TEST(NfcTag_init, IsCalledOnConstruction)
{
    Mock_MFRC522 mfrc;
    EXPECT_CALL(mfrc, initReader()).Times(1);
    NfcTag nfc(&mfrc);
}

TEST_F(NfcTag_Write, invalidFolder_ReturnsFalse)
{
    // Arrange
    //NiceMock<Mock_MFRC522> mfrc; // Calls initReader() on creation which is uninteresting (tested above)
    //NfcTag nfc(&mfrc);
    // Act
    Folder incompleteFolder;
    // Assert
    EXPECT_FALSE(nfc->write_folder_to_card(incompleteFolder));
}
/*
TEST_F(NfcTag_Write, validFolder_writeIsCalled)
{
    // Arrange
    //NiceMock<Mock_MFRC522> mfrc;
    //Mock_Eeprom eeprom;
    // Act
    //Folder testFolder(1, Folder::ALBUM, 5);
    //testFolder.setup_dependencies(&eeprom, 0);
    //NfcTag nfc(&mfrc);
    // Assert
    EXPECT_CALL(mfrc, writeCard(_, _)).Times(1);
    nfc->write_folder_to_card(*testFolder);
}

TEST_F(NfcTag_Write, validFolder_writeIsCalledWithCorrectBlockAddr)
{
    // Arrange
    //NiceMock<Mock_MFRC522> mfrc;
    //Mock_Eeprom eeprom;
    // Act
    //Folder testFolder(1, Folder::ALBUM, 5);
    //testFolder.setup_dependencies(&eeprom, 0);
    //NfcTag nfc(&mfrc);
    // Assert
    EXPECT_CALL(mfrc, writeCard(4, _)).Times(1);
    nfc->write_folder_to_card(*testFolder);
}

MATCHER_P2(HasBytes, bytes, size, "")
{
    for (int i = 0; i < size; ++i)
    {
        if (arg[i] != bytes[i])
        {
            EXPECT_EQ("", "Error: " + std::to_string(i) + "'th element not matching:");
            EXPECT_EQ(arg[i], bytes[i]);
        }
    }
    return true;
}

TEST_F(NfcTag_Write, validFolder_writeIsCalledWithCorrectPayload)
{

    // NfcTag nfc(&mfrc);
    // Assert
    EXPECT_TRUE(testFolder.is_valid());
    EXPECT_CALL(mfrc, writeCard(_, HasBytes(expectedBufferData, 16))).Times(1);
    //EXPECT_THAT(expectedBufferData, HasBytes(expectedBufferData, 16));
    nfc->write_folder_to_card(*testFolder);
}

TEST_F(NfcTag_Write, validFolder_writeSuccess)
{
    // Arrange
    //NiceMock<Mock_MFRC522> mfrc;
    //Mock_Eeprom eeprom;
    // Act
    //Folder testFolder(1, Folder::ALBUM, 5);
    //testFolder.setup_dependencies(&eeprom, 0);
    //NfcTag nfc(&mfrc);
    // Assert
    EXPECT_TRUE(testFolder.is_valid());
    EXPECT_CALL(mfrc, writeCard(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(nfc->write_folder_to_card(*testFolder));
}*/
