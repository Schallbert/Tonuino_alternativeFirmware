#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <NfcTag.h>
#include "mocks/unittest_Folder_mocks.h"
#include "mocks/unittest_NfcTag_mocks.h"
#include "mocks/unittest_NfcTag_fakes.h"
#include "mocks/unittest_NfcTag_fixture.h"

using ::testing::Return;
//using ::testing::AtLeast;
using ::testing::_;
using ::testing::ElementsAre;

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
    Mock_MFRC522 mfrc;
    EXPECT_CALL(mfrc, initReader()).Times(1);
    NfcTag m_pNfc(&mfrc);
}

TEST_F(NfcTagReadWrite, Write_invalidFolder_ReturnsFalse)
{
    Folder incompleteFolder;
    EXPECT_FALSE(incompleteFolder.is_valid());
    EXPECT_FALSE(m_pNfc->write_folder_to_card(incompleteFolder));
}

TEST_F(NfcTagReadWrite, Write_validFolder_IsCalled)
{
    EXPECT_CALL(*m_pMfrc, writeCard(_, _)).Times(1);
    m_pNfc->write_folder_to_card(*m_pTestFolder);
}

TEST_F(NfcTagReadWrite, Write_validFolder_IsCalledWithCorrectBlockAddr)
{
    EXPECT_CALL(*m_pMfrc, writeCard(4, _)).Times(1);
    m_pNfc->write_folder_to_card(*m_pTestFolder);
}

TEST_F(NfcTagReadWrite, Write_validFolder_IsCalledWithCorrectPayload)
{
    EXPECT_TRUE(m_pTestFolder->is_valid());
    EXPECT_CALL(*m_pMfrc, writeCard(_, arrayByteCompare(
                                fakeBufferData,
                                MFRC522_interface::NFCTAG_MEMORY_TO_OCCUPY
                                ))).Times(1);
    m_pNfc->write_folder_to_card(*m_pTestFolder);
}

TEST_F(NfcTagReadWrite, Write_validFolder_writeSuccess)
{
    EXPECT_TRUE(m_pTestFolder->is_valid());
    EXPECT_CALL(*m_pMfrc, writeCard(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(m_pNfc->write_folder_to_card(*m_pTestFolder));
}

TEST_F(NfcTagReadWrite, Read_NotSuccessful_returnsFalse)
{
    Folder resultFolder;
    EXPECT_CALL(*m_pMfrc, readCard(_, _)).WillOnce(Return(false));
    EXPECT_FALSE(m_pNfc->read_folder_from_card(resultFolder));
}

TEST_F(NfcTagReadWrite, Read_Successful_returnsTrue)
{
    Folder resultFolder;
    EXPECT_CALL(*m_pMfrc, readCard(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(m_pNfc->read_folder_from_card(resultFolder));
}

TEST_F(NfcTagReadWrite, Read_IsCalledWithCorrectBlockAddr)
{
    Folder resultFolder;
    EXPECT_CALL(*m_pMfrc, readCard(4, _)).WillOnce(Return(true));
    m_pNfc->read_folder_from_card(resultFolder);
}

TEST_F(NfcTagReadWrite, Read_IsCalledWithCorrectPayload)
{
    Folder resultFolder;
    EXPECT_CALL(*m_pMfrc, readCard(_, arrayByteCompare(
                                  fakeBufferData,
                                  MFRC522_interface::NFCTAG_MEMORY_TO_OCCUPY
                                  )));
    // sets buffer to a certain value
    m_pNfc->write_folder_to_card(*m_pTestFolder);
    // read with this buffer sets correct argument at readCard
    m_pNfc->read_folder_from_card(resultFolder);
}

TEST_F(NfcTagReadWrite, Read_Successful_bufferEmpty_overridesSourceFolder)
{
    Folder resultFolder(27, Folder::LULLABYE, 5);
    EXPECT_CALL(*m_pMfrc, readCard(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(m_pNfc->read_folder_from_card(resultFolder));
    EXPECT_EQ(0, resultFolder.get_folder_id());
    EXPECT_EQ(Folder::UNDEFINED, resultFolder.get_play_mode());
    EXPECT_EQ(0, resultFolder.get_track_count());
}

TEST_F(NfcTagReadWrite, Read_Successful_bufferEmpty_returnsUnknownCard)
{
    Folder resultFolder;
    EXPECT_CALL(*m_pMfrc, readCard(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(m_pNfc->read_folder_from_card(resultFolder));
    EXPECT_FALSE(m_pNfc->is_known_card());
}

TEST_F(NfcTagReadWrite, Read_Successful_bufferSet_returnsKnownCard)
{
    Folder resultFolder;
    m_pMfrc->DelegateToFake(); // Delegates readCard() call to fake object
    EXPECT_CALL(*m_pMfrc, readCard(_, _));

    EXPECT_TRUE(m_pNfc->read_folder_from_card(resultFolder));
    EXPECT_TRUE(m_pNfc->is_known_card());
}

TEST_F(NfcTagReadWrite, Read_Successful_bufferSet_returnsCorrectFolderData)
{
    Folder resultFolder;
    m_pMfrc->DelegateToFake(); // Delegates readCard() call to fake object
    EXPECT_CALL(*m_pMfrc, readCard(_, _));

    EXPECT_TRUE(m_pNfc->read_folder_from_card(resultFolder));
    EXPECT_EQ(m_pTestFolder->get_folder_id(), resultFolder.get_folder_id());
    EXPECT_EQ(m_pTestFolder->get_play_mode(), resultFolder.get_play_mode());
    EXPECT_EQ(m_pTestFolder->get_track_count(), resultFolder.get_track_count());
}