#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mocks/unittest_NfcTag_mocks.h"

class NfcControl;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// TEST FIXTURE
class NfcTagReadWrite : public ::testing::Test
{
protected:
    // Arrange
    virtual void SetUp()
    {
        m_pMfrc = new NiceMock<Mock_MFRC522>;
        m_pNfc = new NfcControl(m_pMfrc);
        m_pTestFolder = new Folder(fakeBufferData[4],
                                   (Folder::ePlayMode)fakeBufferData[5],
                                   fakeBufferData[6]);
    }

    virtual void TearDown()
    {
        delete m_pMfrc;
        delete m_pNfc;
        delete m_pTestFolder;
    }

protected:
    NiceMock<Mock_MFRC522>* m_pMfrc{nullptr};
    NfcControl* m_pNfc{nullptr};
    Folder* m_pTestFolder{nullptr};
};

// TESTS
TEST_F(NfcTagReadWrite, IsCalledOnConstruction)
{
    Mock_MFRC522 mfrc;
    EXPECT_CALL(mfrc, initReader()).Times(1);
    NfcControl m_pNfc(&mfrc);
}

TEST_F(NfcTagReadWrite, Write_invalidFolder_ReturnsFalse)
{
    Folder incompleteFolder;
    EXPECT_FALSE(incompleteFolder.is_valid());
    EXPECT_FALSE(m_pNfc->write_folder_to_card(incompleteFolder));
}

TEST_F(NfcTagReadWrite, Write_validFolder_IsCalled)
{
    EXPECT_CALL(*m_pMfrc, writeTag(_, _)).Times(1);
    m_pNfc->write_folder_to_card(*m_pTestFolder);
}

TEST_F(NfcTagReadWrite, Write_validFolder_IsCalledWithCorrectBlockAddr)
{
    EXPECT_CALL(*m_pMfrc, writeTag(4, _)).Times(1);
    m_pNfc->write_folder_to_card(*m_pTestFolder);
}

TEST_F(NfcTagReadWrite, Write_validFolder_IsCalledWithCorrectPayload)
{
    EXPECT_TRUE(m_pTestFolder->is_valid());
    EXPECT_CALL(*m_pMfrc, writeTag(_, arrayByteCompare(
                                fakeBufferData,
                                MFRC522_interface::NFCTAG_MEMORY_TO_OCCUPY
                                ))).Times(1);
    m_pNfc->write_folder_to_card(*m_pTestFolder);
}

TEST_F(NfcTagReadWrite, Write_validFolder_writeSuccess)
{
    EXPECT_TRUE(m_pTestFolder->is_valid());
    EXPECT_CALL(*m_pMfrc, writeTag(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(m_pNfc->write_folder_to_card(*m_pTestFolder));
}

TEST_F(NfcTagReadWrite, Read_NotSuccessful_returnsFalse)
{
    Folder resultFolder;
    EXPECT_CALL(*m_pMfrc, readTag(_, _)).WillOnce(Return(false));
    EXPECT_FALSE(m_pNfc->read_folder_from_card(resultFolder));
}

TEST_F(NfcTagReadWrite, Read_Successful_ReadFolderNotInitialized_returnsFalse)
{
    Folder resultFolder;
    EXPECT_CALL(*m_pMfrc, readTag(_, _)).WillOnce(Return(true));
    EXPECT_FALSE(m_pNfc->read_folder_from_card(resultFolder));
}

TEST_F(NfcTagReadWrite, Read_IsCalledWithCorrectBlockAddr)
{
    Folder resultFolder;
    EXPECT_CALL(*m_pMfrc, readTag(4, _)).WillOnce(Return(true));
    m_pNfc->read_folder_from_card(resultFolder);
}

TEST_F(NfcTagReadWrite, Read_IsCalledWithCorrectPayload)
{
    Folder resultFolder;
    EXPECT_CALL(*m_pMfrc, readTag(_, arrayByteCompare(
                                  fakeBufferData,
                                  MFRC522_interface::NFCTAG_MEMORY_TO_OCCUPY
                                  )));
    // sets buffer to a certain value
    m_pNfc->write_folder_to_card(*m_pTestFolder);
    // read with this buffer sets correct argument at readTag
    m_pNfc->read_folder_from_card(resultFolder);
}

TEST_F(NfcTagReadWrite, Read_Successful_bufferEmpty_NoOverwriteOfSourceFolder)
{
    Folder resultFolder(27, Folder::LULLABYE, 5);
    EXPECT_CALL(*m_pMfrc, readTag(_, _)).WillOnce(Return(true));
    EXPECT_FALSE(m_pNfc->read_folder_from_card(resultFolder));
    EXPECT_EQ(27, resultFolder.get_folder_id());
    EXPECT_EQ(Folder::LULLABYE, resultFolder.get_play_mode());
    EXPECT_EQ(5, resultFolder.get_track_count());
}

TEST_F(NfcTagReadWrite, Read_Successful_bufferSet_returnsKnownCard)
{
    Folder resultFolder;
    m_pMfrc->DelegateToFake(); // Delegates readTag() call to fake object
    EXPECT_CALL(*m_pMfrc, readTag(_, _));

    EXPECT_TRUE(m_pNfc->read_folder_from_card(resultFolder));
    EXPECT_TRUE(m_pNfc->is_known_card());
}

TEST_F(NfcTagReadWrite, Read_Successful_bufferSet_returnsCorrectFolderData)
{
    Folder resultFolder;
    m_pMfrc->DelegateToFake(); // Delegates readTag() call to fake object
    EXPECT_CALL(*m_pMfrc, readTag(_, _));

    EXPECT_TRUE(m_pNfc->read_folder_from_card(resultFolder));
    EXPECT_EQ(m_pTestFolder->get_folder_id(), resultFolder.get_folder_id());
    EXPECT_EQ(m_pTestFolder->get_play_mode(), resultFolder.get_play_mode());
    EXPECT_EQ(m_pTestFolder->get_track_count(), resultFolder.get_track_count());
}

TEST_F(NfcTagReadWrite, Erase)
{
    // Compare if input of writeTag buffer is really 0
    byte emptyBuffer[MFRC522_interface::NFCTAG_MEMORY_TO_OCCUPY] = {};
    EXPECT_CALL(*m_pMfrc, writeTag(_, arrayByteCompare(
                                  emptyBuffer,
                                  MFRC522_interface::NFCTAG_MEMORY_TO_OCCUPY
                                  )));
    m_pNfc->erase_card();
}