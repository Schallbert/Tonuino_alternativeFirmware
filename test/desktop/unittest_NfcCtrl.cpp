#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Arduino_config.h"
#include "Tonuino_config.h"
#include "../Nfc/NfcControl/NfcControl.h"

#include "mocks/unittest_MessageHandler_mocks.h"
#include "mocks/unittest_Nfc_mocks.h"
#include "mocks/unittest_ArduinoDIcontainer_mocks.h"


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
        m_pNfcControl = new NfcControl(m_nfcMock, m_messageHandlerMock);
        m_pTestFolder = new Folder(fakeBufferData[4],
                                   (Folder::ePlayMode)fakeBufferData[5]);
        m_pTestFolder->setTrackCount(fakeBufferData[6]);
        m_pTestFolder->setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    }

    virtual void TearDown()
    {
        delete m_pNfcControl;
        delete m_pTestFolder;
    }

protected:
    NiceMock<Mock_Nfc> m_nfcMock{};
    NiceMock<Mock_MessageHandler> m_messageHandlerMock{};
    NiceMock<Mock_ArduinoDIcontainer> m_arduinoHalMock{};
    NfcControl *m_pNfcControl{nullptr};
    Folder *m_pTestFolder{nullptr};
};

class NfcCtrlRead : public NfcCtrlWrite
{
};
class NfcCtrlTagPresence : public NfcCtrlWrite
{
};
class NfcCtrlDebugOutput : public NfcCtrlWrite
{
};

// TESTS
TEST_F(NfcCtrlWrite, initNfc_IsCalledOnConstruction)
{
    Mock_Nfc nfc;
    EXPECT_CALL(nfc, initNfc()).Times(1);
    NfcControl m_pNfcControl(nfc, m_messageHandlerMock);
}

TEST_F(NfcCtrlWrite, invalidFolder_ReturnsFalse)
{
    Folder incompleteFolder;
    EXPECT_FALSE(m_pNfcControl->writeFolderToTag(incompleteFolder));
}

TEST_F(NfcCtrlWrite, validFolder_IsCalled)
{
    EXPECT_CALL(m_nfcMock, writeTag(_, _)).Times(1);
    m_pNfcControl->writeFolderToTag(*m_pTestFolder);
}

TEST_F(NfcCtrlWrite, validFolder_IsCalledWithCorrectBlockAddr)
{
    EXPECT_CALL(m_nfcMock, writeTag(4, _)).Times(1);
    m_pNfcControl->writeFolderToTag(*m_pTestFolder);
}

TEST_F(NfcCtrlWrite, validFolder_IsCalledWithCorrectPayload)
{
    EXPECT_CALL(m_nfcMock, writeTag(_, arrayByteCompare(
                                         fakeBufferData,
                                         NFCTAG_MEMORY_TO_OCCUPY)))
        .Times(1);
    m_pNfcControl->writeFolderToTag(*m_pTestFolder);
}

TEST_F(NfcCtrlWrite, validFolder_writeSuccess)
{
    ON_CALL(m_nfcMock, writeTag(_, _)).WillByDefault(Return(true));
    EXPECT_TRUE(m_pNfcControl->writeFolderToTag(*m_pTestFolder));
}

TEST_F(NfcCtrlWrite, EraseTag)
{
    // Compare if input of writeTag buffer is really 0
    byte emptyBuffer[NFCTAG_MEMORY_TO_OCCUPY] = {};
    EXPECT_CALL(m_nfcMock, writeTag(_, arrayByteCompare(
                                         emptyBuffer,
                                         NFCTAG_MEMORY_TO_OCCUPY)));
    m_pNfcControl->eraseTag();
}

TEST_F(NfcCtrlRead, Read_NotSuccessful_returnsFalse)
{
    Folder resultFolder;
    ON_CALL(m_nfcMock, readTag(_, _)).WillByDefault(Return(false));
    EXPECT_FALSE(m_pNfcControl->readFolderFromTag(resultFolder));
}

TEST_F(NfcCtrlRead, Read_Successful_NoDataToRead_returnsFalse)
{
    Folder resultFolder;
    ON_CALL(m_nfcMock, readTag(_, _)).WillByDefault(Return(true));
    EXPECT_FALSE(m_pNfcControl->readFolderFromTag(resultFolder));
}

TEST_F(NfcCtrlRead, isCalledWithCorrectBlockAddr)
{
    Folder resultFolder;
    ON_CALL(m_nfcMock, readTag(4, _)).WillByDefault(Return(true));
    m_pNfcControl->readFolderFromTag(resultFolder);
}

TEST_F(NfcCtrlRead, isCalledWithCorrectPayload)
{
    Folder resultFolder;
    EXPECT_CALL(m_nfcMock, readTag(_, arrayByteCompare(
                                        fakeBufferData,
                                        NFCTAG_MEMORY_TO_OCCUPY)));
    // sets buffer to a certain value
    m_pNfcControl->writeFolderToTag(*m_pTestFolder);
    // read with this buffer sets correct argument at readTag
    m_pNfcControl->readFolderFromTag(resultFolder);
}

TEST_F(NfcCtrlRead, Read_Successful_bufferEmpty_NoOverwriteOfSourceFolder)
{
    Folder resultFolder(27, Folder::ONELARGETRACK);
    resultFolder.setTrackCount(5);
    ON_CALL(m_nfcMock, readTag(_, _)).WillByDefault(Return(true));

    EXPECT_FALSE(m_pNfcControl->readFolderFromTag(resultFolder));
    EXPECT_EQ(27, resultFolder.getFolderId());
    EXPECT_EQ(Folder::ONELARGETRACK, resultFolder.getPlayMode());
    EXPECT_EQ(5, resultFolder.getTrackCount());
}

TEST_F(NfcCtrlRead, Read_Successful_bufferSet_returnsCorrectFolderData)
{
    Folder resultFolder;
    m_nfcMock.DelegateToFake(); // Delegates readTag() call to fake object
    EXPECT_TRUE(m_pNfcControl->readFolderFromTag(resultFolder));

    EXPECT_EQ(m_pTestFolder->getFolderId(), resultFolder.getFolderId());
    EXPECT_EQ(m_pTestFolder->getPlayMode(), resultFolder.getPlayMode());
    EXPECT_EQ(m_pTestFolder->getTrackCount(), resultFolder.getTrackCount());
}

TEST_F(NfcCtrlTagPresence, noTag_returnsNoTag)
{
    NfcControl_interface::eTagState tagPresence = NfcControl_interface::NO_TAG;
    ON_CALL(m_nfcMock, getTagPresence()).WillByDefault(Return(tagPresence));
    ASSERT_EQ(tagPresence, m_pNfcControl->getTagPresence());
}

TEST_F(NfcCtrlTagPresence, activeTag_returnsActiveTag)
{
    NfcControl_interface::eTagState tagPresence = NfcControl_interface::ACTIVE_KNOWN_TAG;
    ON_CALL(m_nfcMock, getTagPresence()).WillByDefault(Return(tagPresence));
    ASSERT_EQ(tagPresence, m_pNfcControl->getTagPresence());
}

TEST_F(NfcCtrlTagPresence, newTag_simulateUnknown_returnsUnknownTag)
{
    NfcControl_interface::eTagState tagPresence = NfcControl_interface::NEW_UNKNOWN_TAG;
    ON_CALL(m_nfcMock, getTagPresence()).WillByDefault(Return(tagPresence));
    ASSERT_EQ(tagPresence, m_pNfcControl->getTagPresence());
}

TEST_F(NfcCtrlTagPresence, newTag_simulateKnown_returnsKnownTag)
{
    NfcControl myTest{m_nfcMock, m_messageHandlerMock};
    NfcControl_interface::eTagState tagPresence = NfcControl_interface::NEW_UNKNOWN_TAG;
    ON_CALL(m_nfcMock, getTagPresence()).WillByDefault(Return(tagPresence));
    m_nfcMock.DelegateToFake(); // will return known card cookie
    ASSERT_EQ(NfcControl_interface::NEW_REGISTERED_TAG, myTest.getTagPresence());
}

TEST_F(NfcCtrlTagPresence, OutOfRange_returnsOutOfRange)
{
    NfcControl_interface::eTagState tagPresence = static_cast<NfcControl_interface::eTagState>(static_cast<uint8_t>(NfcControl_interface::NUMBER_OF_TAG_STATES) + 1);
    ON_CALL(m_nfcMock, getTagPresence()).WillByDefault(Return(tagPresence));
    ASSERT_EQ(tagPresence, m_pNfcControl->getTagPresence());
}