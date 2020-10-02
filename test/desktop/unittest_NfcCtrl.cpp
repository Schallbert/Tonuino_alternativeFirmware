#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Arduino_config.h"
#include "mocks/unittest_ArduinoIf_mocks.h"
#include "../Nfc/NfcControl/NfcControl.h"
#include "mocks/unittest_Nfc_mocks.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// TEST FIXTURE
class NfcCtrlWrite : public ::testing::Test
{
protected:
    // Arrange
    virtual void SetUp()
    {
        m_pNfc = new NiceMock<Mock_Nfc>;
        m_pSerial = new NiceMock<Mock_serial>;
        m_pNfcCtrl = new NfcControl(m_pNfc, m_pSerial);
        m_pTestFolder = new Folder(fakeBufferData[4],
                                   (Folder::ePlayMode)fakeBufferData[5],
                                   fakeBufferData[6]);
    }

    virtual void TearDown()
    {
        
        delete m_pNfcCtrl;
        delete m_pNfc;
        delete m_pSerial;
        delete m_pTestFolder;
    }

protected:
    Arduino_interface_com *m_pSerial{nullptr};
    NiceMock<Mock_Nfc>* m_pNfc{nullptr};
    NfcControl* m_pNfcCtrl{nullptr};
    Folder* m_pTestFolder{nullptr};
};

class NfcCtrlRead : public NfcCtrlWrite{};
class NfcCtrlTagPresence : public NfcCtrlWrite{};

// TESTS
TEST_F(NfcCtrlWrite, initNfc_IsCalledOnConstruction)
{
    Mock_Nfc nfc;
    EXPECT_CALL(nfc, initNfc()).Times(1);
    NfcControl m_pNfcCtrl(&nfc, m_pSerial);
}

TEST_F(NfcCtrlWrite, invalidFolder_ReturnsFalse)
{
    Folder incompleteFolder;
    EXPECT_FALSE(m_pNfcCtrl->write_folder_to_card(incompleteFolder));
}

TEST_F(NfcCtrlWrite, validFolder_IsCalled)
{
    EXPECT_CALL(*m_pNfc, writeTag(_, _)).Times(1);
    m_pNfcCtrl->write_folder_to_card(*m_pTestFolder);
}

TEST_F(NfcCtrlWrite, validFolder_IsCalledWithCorrectBlockAddr)
{
    EXPECT_CALL(*m_pNfc, writeTag(4, _)).Times(1);
    m_pNfcCtrl->write_folder_to_card(*m_pTestFolder);
}

TEST_F(NfcCtrlWrite, validFolder_IsCalledWithCorrectPayload)
{
    EXPECT_CALL(*m_pNfc, writeTag(_, arrayByteCompare(
                                fakeBufferData,
                                NFCTAG_MEMORY_TO_OCCUPY
                                ))).Times(1);
    m_pNfcCtrl->write_folder_to_card(*m_pTestFolder);
}

TEST_F(NfcCtrlWrite, validFolder_writeSuccess)
{
    ON_CALL(*m_pNfc, writeTag(_, _)).WillByDefault(Return(true));
    EXPECT_TRUE(m_pNfcCtrl->write_folder_to_card(*m_pTestFolder));
}

TEST_F(NfcCtrlWrite, EraseTag)
{
    // Compare if input of writeTag buffer is really 0
    byte emptyBuffer[NFCTAG_MEMORY_TO_OCCUPY] = {};
    EXPECT_CALL(*m_pNfc, writeTag(_, arrayByteCompare(
                                  emptyBuffer,
                                  NFCTAG_MEMORY_TO_OCCUPY
                                  )));
    m_pNfcCtrl->erase_card();
}

TEST_F(NfcCtrlRead, Read_NotSuccessful_returnsFalse)
{
    Folder resultFolder;
    ON_CALL(*m_pNfc, readTag(_, _)).WillByDefault(Return(false));
    EXPECT_FALSE(m_pNfcCtrl->read_folder_from_card(resultFolder));
}

TEST_F(NfcCtrlRead, Read_Successful_ReadFolderNotInitialized_returnsFalse)
{
    Folder resultFolder;
    ON_CALL(*m_pNfc, readTag(_, _)).WillByDefault(Return(true));
    EXPECT_FALSE(m_pNfcCtrl->read_folder_from_card(resultFolder));
}

TEST_F(NfcCtrlRead, isCalledWithCorrectBlockAddr)
{
    Folder resultFolder;
    ON_CALL(*m_pNfc, readTag(4, _)).WillByDefault(Return(true));
    m_pNfcCtrl->read_folder_from_card(resultFolder);
}

TEST_F(NfcCtrlRead, isCalledWithCorrectPayload)
{
    Folder resultFolder;
    EXPECT_CALL(*m_pNfc, readTag(_, arrayByteCompare(
                                  fakeBufferData,
                                  NFCTAG_MEMORY_TO_OCCUPY
                                  )));
    // sets buffer to a certain value
    m_pNfcCtrl->write_folder_to_card(*m_pTestFolder);
    // read with this buffer sets correct argument at readTag
    m_pNfcCtrl->read_folder_from_card(resultFolder);
}

TEST_F(NfcCtrlRead, Read_Successful_bufferEmpty_NoOverwriteOfSourceFolder)
{
    Folder resultFolder(27, Folder::LULLABYE, 5);
    ON_CALL(*m_pNfc, readTag(_, _)).WillByDefault(Return(true));
    EXPECT_FALSE(m_pNfcCtrl->read_folder_from_card(resultFolder));
    EXPECT_EQ(27, resultFolder.get_folder_id());
    EXPECT_EQ(Folder::LULLABYE, resultFolder.get_play_mode());
    EXPECT_EQ(5, resultFolder.get_track_count());
}

TEST_F(NfcCtrlRead, Read_Successful_bufferSet_returnsCorrectFolderData)
{
    Folder resultFolder;
    m_pNfc->DelegateToFake(); // Delegates readTag() call to fake object
    EXPECT_TRUE(m_pNfcCtrl->read_folder_from_card(resultFolder));

    EXPECT_EQ(m_pTestFolder->get_folder_id(), resultFolder.get_folder_id());
    EXPECT_EQ(m_pTestFolder->get_play_mode(), resultFolder.get_play_mode());
    EXPECT_EQ(m_pTestFolder->get_track_count(), resultFolder.get_track_count());
}

TEST_F(NfcCtrlTagPresence, noTag_returnsNoTag)
{
    Nfc_interface::eTagState nfcTagPresence = Nfc_interface::NO_TAG;
    ON_CALL(*m_pNfc, getTagPresence()).WillByDefault(Return(nfcTagPresence));
    EXPECT_EQ(nfcTagPresence, m_pNfcCtrl->get_tag_presence());
}

TEST_F(NfcCtrlTagPresence, activeTag_returnsActiveTag)
{
    Nfc_interface::eTagState nfcTagPresence = Nfc_interface::ACTIVE_KNOWN_TAG;
    ON_CALL(*m_pNfc, getTagPresence()).WillByDefault(Return(nfcTagPresence));
    EXPECT_EQ(nfcTagPresence, m_pNfcCtrl->get_tag_presence());
}

TEST_F(NfcCtrlTagPresence, newTag_simulateUnknown_returnsUnknownTag)
{
    Nfc_interface::eTagState nfcTagPresence = Nfc_interface::NEW_TAG;
    ON_CALL(*m_pNfc, getTagPresence()).WillByDefault(Return(nfcTagPresence));
    EXPECT_EQ(Nfc_interface::NEW_UNKNOWN_TAG, m_pNfcCtrl->get_tag_presence());
}

TEST_F(NfcCtrlTagPresence, newTag_simulateKnown_returnsKnownTag)
{
    Nfc_interface::eTagState nfcTagPresence = Nfc_interface::NEW_TAG;
    ON_CALL(*m_pNfc, getTagPresence()).WillByDefault(Return(nfcTagPresence));
    m_pNfc->DelegateToFake(); // will return known card cookie
    EXPECT_EQ(Nfc_interface::NEW_KNOWN_TAG, m_pNfcCtrl->get_tag_presence());
}

TEST_F(NfcCtrlTagPresence, OutOfRange_returnsOutOfRange)
{
    Nfc_interface::eTagState nfcTagPresence = static_cast<Nfc_interface::eTagState>(6);
    ON_CALL(*m_pNfc, getTagPresence()).WillByDefault(Return(nfcTagPresence));
    EXPECT_EQ(nfcTagPresence, m_pNfcCtrl->get_tag_presence());
}