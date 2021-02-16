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
        m_TestFolder.setTrackCount(fakeBufferData[6]);
        m_TestFolder.setupDependencies(&m_arduinoHalMock, &m_messageHandlerMock);
    }

protected:
    NiceMock<Mock_Nfc> m_nfcMock;
    NiceMock<Mock_MessageHandler> m_messageHandlerMock{};
    NiceMock<Mock_ArduinoDIcontainer> m_arduinoHalMock{};
    NfcControl m_NfcControl{m_nfcMock, m_messageHandlerMock};
    Folder m_TestFolder{fakeBufferData[4],
                        (Folder::ePlayMode)fakeBufferData[5]};
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
    EXPECT_CALL(m_nfcMock, initNfc());
    m_NfcControl.init();
}

TEST_F(NfcCtrlWrite, invalidFolder_ReturnsFalse)
{
    Folder incompleteFolder;
    EXPECT_FALSE(m_NfcControl.writeFolderToTag(incompleteFolder));
}

TEST_F(NfcCtrlWrite, validFolder_IsCalled)
{
    EXPECT_CALL(m_nfcMock, writeTag(_, _)).Times(1);
    m_NfcControl.writeFolderToTag(m_TestFolder);
}

TEST_F(NfcCtrlWrite, validFolder_IsCalledWithCorrectBlockAddr)
{
    EXPECT_CALL(m_nfcMock, writeTag(4, _)).Times(1);
    m_NfcControl.writeFolderToTag(m_TestFolder);
}

TEST_F(NfcCtrlWrite, validFolder_IsCalledWithCorrectPayload)
{
    EXPECT_CALL(m_nfcMock, writeTag(_, arrayByteCompare(
                                           fakeBufferData,
                                           NFCTAG_MEMORY_TO_OCCUPY)))
        .Times(1);
    m_NfcControl.writeFolderToTag(m_TestFolder);
}

TEST_F(NfcCtrlWrite, validFolder_writeSuccess)
{
    ON_CALL(m_nfcMock, writeTag(_, _)).WillByDefault(Return(true));
    EXPECT_TRUE(m_NfcControl.writeFolderToTag(m_TestFolder));
}

TEST_F(NfcCtrlWrite, EraseTag)
{
    // Compare if input of writeTag buffer is really 0
    byte emptyBuffer[NFCTAG_MEMORY_TO_OCCUPY] = {};
    EXPECT_CALL(m_nfcMock, writeTag(_, arrayByteCompare(
                                           emptyBuffer,
                                           NFCTAG_MEMORY_TO_OCCUPY)));
    m_NfcControl.eraseTag();
}

TEST_F(NfcCtrlRead, Read_NotSuccessful_returnsFalse)
{
    Folder resultFolder;
    ON_CALL(m_nfcMock, readTag(_, _)).WillByDefault(Return(false));
    EXPECT_FALSE(m_NfcControl.readFolderFromTag(resultFolder));
}

TEST_F(NfcCtrlRead, Read_Successful_NoDataToRead_returnsFalse)
{
    Folder resultFolder;
    ON_CALL(m_nfcMock, readTag(_, _)).WillByDefault(Return(true));
    EXPECT_FALSE(m_NfcControl.readFolderFromTag(resultFolder));
}

TEST_F(NfcCtrlRead, isCalledWithCorrectBlockAddr)
{
    Folder resultFolder;
    ON_CALL(m_nfcMock, readTag(4, _)).WillByDefault(Return(true));
    m_NfcControl.readFolderFromTag(resultFolder);
}

TEST_F(NfcCtrlRead, isCalledWithCorrectPayload)
{
    Folder resultFolder;
    EXPECT_CALL(m_nfcMock, readTag(_, arrayByteCompare(
                                          fakeBufferData,
                                          NFCTAG_MEMORY_TO_OCCUPY)));
    // sets buffer to a certain value
    m_NfcControl.writeFolderToTag(m_TestFolder);
    // read with this buffer sets correct argument at readTag
    m_NfcControl.readFolderFromTag(resultFolder);
}

TEST_F(NfcCtrlRead, Read_Successful_bufferEmpty_NoOverwriteOfSourceFolder)
{
    Folder resultFolder(27, Folder::ONELARGETRACK);
    resultFolder.setTrackCount(5);
    ON_CALL(m_nfcMock, readTag(_, _)).WillByDefault(Return(true));

    EXPECT_FALSE(m_NfcControl.readFolderFromTag(resultFolder));
    EXPECT_EQ(27, resultFolder.getFolderId());
    EXPECT_EQ(Folder::ONELARGETRACK, resultFolder.getPlayMode());
    EXPECT_EQ(5, resultFolder.getTrackCount());
}

TEST_F(NfcCtrlRead, Read_Successful_bufferSet_returnsCorrectFolderData)
{
    Folder resultFolder;
    m_nfcMock.DelegateToFake(); // Delegates readTag() call to fake object
    EXPECT_TRUE(m_NfcControl.readFolderFromTag(resultFolder));

    EXPECT_EQ(m_TestFolder.getFolderId(), resultFolder.getFolderId());
    EXPECT_EQ(m_TestFolder.getPlayMode(), resultFolder.getPlayMode());
    EXPECT_EQ(m_TestFolder.getTrackCount(), resultFolder.getTrackCount());
}

TEST_F(NfcCtrlTagPresence, noTag_returnsNoTag)
{
    Message::eMessageContent tagPresence = Message::NOTAG;
    ON_CALL(m_nfcMock, getTagPresence()).WillByDefault(Return(tagPresence));
    ASSERT_EQ(tagPresence, m_NfcControl.getTagPresence());
}

TEST_F(NfcCtrlTagPresence, activeTag_returnsActiveTag)
{
    Message::eMessageContent tagPresence = Message::ACTIVETAG;
    ON_CALL(m_nfcMock, getTagPresence()).WillByDefault(Return(tagPresence));
    ASSERT_EQ(tagPresence, m_NfcControl.getTagPresence());
}

TEST_F(NfcCtrlTagPresence, newTag_simulateUnknown_returnsUnknownTag)
{
    Message::eMessageContent tagPresence = Message::UNKNOWNTAG;
    ON_CALL(m_nfcMock, getTagPresence()).WillByDefault(Return(tagPresence));
    ASSERT_EQ(tagPresence, m_NfcControl.getTagPresence());
}

TEST_F(NfcCtrlTagPresence, newTag_simulateKnown_returnsKnownTag)
{
    NfcControl myTest{m_nfcMock, m_messageHandlerMock};
    Message::eMessageContent tagPresence = Message::UNKNOWNTAG;
    ON_CALL(m_nfcMock, getTagPresence()).WillByDefault(Return(tagPresence));
    m_nfcMock.DelegateToFake(); // will return known card cookie
    ASSERT_EQ(Message::NEWKNOWNTAG, myTest.getTagPresence());
}

TEST_F(NfcCtrlTagPresence, OutOfRange_returnsOutOfRange)
{
    Message::eMessageContent tagPresence = Message::READERINIT; // Out of Range
    ON_CALL(m_nfcMock, getTagPresence()).WillByDefault(Return(tagPresence));
    ASSERT_EQ(tagPresence, m_NfcControl.getTagPresence());
}