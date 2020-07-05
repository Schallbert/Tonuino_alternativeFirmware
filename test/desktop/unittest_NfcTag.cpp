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
class NfcTagReadWrite : public ::testing::Test
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

// TESTS
TEST_F(NfcTagReadWrite, IsCalledOnConstruction)
{
    Mock_MFRC522 mfrc1;
    EXPECT_CALL(mfrc1, initReader()).Times(1);
    NfcTag nfc(&mfrc1);
}

TEST_F(NfcTagReadWrite, Write_invalidFolder_ReturnsFalse)
{
    Folder incompleteFolder;
    EXPECT_FALSE(incompleteFolder.is_valid());
    EXPECT_FALSE(nfc->write_folder_to_card(incompleteFolder));
}

TEST_F(NfcTagReadWrite, Write_validFolder_IsCalled)
{
    EXPECT_CALL(*mfrc, writeCard(_, _)).Times(1);
    nfc->write_folder_to_card(*testFolder);
}

TEST_F(NfcTagReadWrite, Write_validFolder_IsCalledWithCorrectBlockAddr)
{
    EXPECT_CALL(*mfrc, writeCard(4, _)).Times(1);
    nfc->write_folder_to_card(*testFolder);
}

TEST_F(NfcTagReadWrite, Write_validFolder_IsCalledWithCorrectPayload)
{
    EXPECT_TRUE(testFolder->is_valid());
    EXPECT_CALL(*mfrc, writeCard(_, arrayByteCompare(
                                expectedBufferData,
                                MFRC522_interface::NFCTAG_MEMORY_TO_OCCUPY
                                ))).Times(1);
    nfc->write_folder_to_card(*testFolder);
}

TEST_F(NfcTagReadWrite, Write_validFolder_writeSuccess)
{
    EXPECT_TRUE(testFolder->is_valid());
    EXPECT_CALL(*mfrc, writeCard(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(nfc->write_folder_to_card(*testFolder));
}

TEST_F(NfcTagReadWrite, Read_NotSuccessful_returnsFalse)
{
    Folder resultFolder;
    EXPECT_CALL(*mfrc, readCard(_, _)).WillOnce(Return(false));
    EXPECT_FALSE(nfc->read_folder_from_card(resultFolder));
}

TEST_F(NfcTagReadWrite, Read_Successful_returnsTrue)
{
    Folder resultFolder;
    EXPECT_CALL(*mfrc, readCard(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(nfc->read_folder_from_card(resultFolder));
}

TEST_F(NfcTagReadWrite, Read_IsCalledWithCorrectBlockAddr)
{
    Folder resultFolder;
    EXPECT_CALL(*mfrc, readCard(4, _)).WillOnce(Return(true));
    nfc->read_folder_from_card(resultFolder);
}

TEST_F(NfcTagReadWrite, Read_IsCalledWithCorrectPayload)
{
    Folder resultFolder;
    EXPECT_CALL(*mfrc, readCard(_, arrayByteCompare(
                                  expectedBufferData,
                                  MFRC522_interface::NFCTAG_MEMORY_TO_OCCUPY
                                  ))).WillOnce(Return(true));
    // sets buffer to a certain value
    nfc->write_folder_to_card(*testFolder);
    // read with this buffer sets correct argument at readCard
    nfc->read_folder_from_card(resultFolder);
}

TEST_F(NfcTagReadWrite, Read_Successful_bufferEmpty_overridesSourceFolder)
{
    Folder resultFolder(27, Folder::LULLABYE, 5);
    EXPECT_CALL(*mfrc, readCard(_, _)).WillOnce(Return(true));
    nfc->read_folder_from_card(resultFolder);
    EXPECT_EQ(0, resultFolder.get_folder_id());
    EXPECT_EQ(Folder::UNDEFINED, resultFolder.get_play_mode());
    EXPECT_EQ(0, resultFolder.get_track_count());
}

TEST_F(NfcTagReadWrite, Read_Successful_bufferEmpty_returnsUnknownCard)
{
    Folder resultFolder;
    EXPECT_CALL(*mfrc, readCard(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(nfc->read_folder_from_card(resultFolder));
    EXPECT_FALSE(nfc->is_known_card());
}

TEST_F(NfcTagReadWrite, Read_Successful_bufferSet_returnsKnownCard)
{
    Folder resultFolder;
    ON_CALL(*mfrc, writeCard(_, _)).WillByDefault(Return(true));
    EXPECT_CALL(*mfrc, readCard(_, _)).WillOnce(Return(true));
    // sets buffer to a certain value
    nfc->write_folder_to_card(*testFolder);
    // read works with buffer
    EXPECT_TRUE(nfc->read_folder_from_card(resultFolder));
    EXPECT_TRUE(nfc->is_known_card());
}

TEST_F(NfcTagReadWrite, Read_Successful_bufferSet_returnsCorrectFolderData)
{
    Folder resultFolder;
    ON_CALL(*mfrc, writeCard(_, _)).WillByDefault(Return(true));
    EXPECT_CALL(*mfrc, readCard(_, _)).WillOnce(Return(true));
    // sets buffer to a certain value
    nfc->write_folder_to_card(*testFolder);
    // read works with buffer
    EXPECT_TRUE(nfc->read_folder_from_card(resultFolder));
    EXPECT_EQ(testFolder->get_folder_id(), resultFolder.get_folder_id());
    EXPECT_EQ(testFolder->get_play_mode(), resultFolder.get_play_mode());
    EXPECT_EQ(testFolder->get_track_count(), resultFolder.get_track_count());
}