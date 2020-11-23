#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../Mp3/Mp3Play/Mp3Play_implementation.h"
#include "../Utilities/SimpleTimer.h"

#include "mocks/unittest_ArduinoDIcontainer_mocks.h"
#include "mocks/unittest_ArduinoIf_mocks.h"
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_ErrorHandler_mocks.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// Fixture
class Mp3PlayTest : public ::testing::Test
{
protected:
    // Arrange
    virtual void SetUp()
    {
        m_arduinoHalMock.DelegateToMockPins(&pinControlMock);
        m_pMp3Play = new Mp3Play_implementation(&m_arduinoHalMock,
                                 &m_dfMiniMock,
                                 &m_lullabyeTimer,
                                 &m_errorHandlerMock);
    }

    virtual void TearDown()
    {
        delete m_pMp3Play;
    }

protected:
    NiceMock<Mock_ArduinoDIcontainer> m_arduinoHalMock{};
    NiceMock<Mock_DfMiniMp3> m_dfMiniMock{};
    SimpleTimer m_lullabyeTimer{};
    NiceMock<Mock_ErrorHandler> m_errorHandlerMock{};
    NiceMock<Mock_pinCtrl> pinControlMock{};

    Mp3Play_implementation *m_pMp3Play{nullptr};
};

TEST_F(Mp3PlayTest, isPlaying_notPlaying_returnsFalse)
{
    EXPECT_CALL(pinControlMock, digital_read(_)).WillOnce(Return(true));
    
    ASSERT_FALSE(m_pMp3Play->isPlaying());
}

TEST_F(Mp3PlayTest, isPlaying_Playing_returnsTrue)
{
    EXPECT_CALL(pinControlMock, digital_read(_)).WillOnce(Return(false));
    
    ASSERT_TRUE(m_pMp3Play->isPlaying());
}

TEST_F(Mp3PlayTest, playFolder_notInitialized_setsFolderError)
{
    Folder unInitializedFolder;

    EXPECT_CALL(m_errorHandlerMock, setFolderError());
    m_pMp3Play->playFolder(unInitializedFolder);
}

TEST_F(Mp3PlayTest, playFolder_playerCannotFindFolderOnSdCard_setsFolderError)
{
    Folder nonExistantFolder(1, Folder::ALBUM);
    ON_CALL(m_dfMiniMock, getFolderTrackCount(_)).WillByDefault(Return(0));
     
    EXPECT_CALL(m_errorHandlerMock, setFolderError());
    m_pMp3Play->playFolder(nonExistantFolder);
}

TEST_F(Mp3PlayTest, playFolder_folderValid_playsFolder)
{
    Folder validFolder(1, Folder::ALBUM);
    ON_CALL(m_dfMiniMock, getFolderTrackCount(_)).WillByDefault(Return(1));
     
    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _));
    m_pMp3Play->playFolder(validFolder);
}

TEST_F(Mp3PlayTest, playFolder_folderValid_notifiesPlaying)
{
    Folder validFolder(1, Folder::ALBUM);
    ON_CALL(m_dfMiniMock, getFolderTrackCount(_)).WillByDefault(Return(1));
     
    EXPECT_CALL(m_errorHandlerMock, setMp3ControlNotify(Mp3ControlNotify::play));
    m_pMp3Play->playFolder(validFolder);
}

TEST_F(Mp3PlayTest, playFolder_callTwice_wontPlayAgain)
{
    Folder validFolder(1, Folder::ALBUM);
    ON_CALL(m_dfMiniMock, getFolderTrackCount(_)).WillByDefault(Return(1));
     
    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _)).Times(1);
    m_pMp3Play->playFolder(validFolder);
    m_pMp3Play->playFolder(validFolder);
}



// PROMPT TESTS
#if 0
TEST_F(Mp3PlayTest, playPrompt_noSkip_notPlaying_Timeout)
{
    VoicePrompt prompt;
    prompt.allowSkip = false;
    prompt.promptId = MSG_CONFIRMED;

    ON_CALL(*m_pPinCtrl, digital_read(_)).WillByDefault(Return(true));                                            // not playing                                                               // not playing
    ON_CALL(m_dfMiniMock, loop()).WillByDefault(InvokeWithoutArgs(m_pDfMiniMsgTimeout, &SimpleTimer::timerTick)); //called twice before timeout

    m_pMp3Control->play_prompt(prompt);
}

TEST_F(Mp3PlayTest, playPrompt_noSkip_notFinishing_Timeout)
{
    VoicePrompt prompt;
    prompt.allowSkip = false;
    prompt.promptId = MSG_CONFIRMED;

    ON_CALL(*m_pPinCtrl, digital_read(_)).WillByDefault(Return(false));                                           // playing                                                                           // not playing
    ON_CALL(m_dfMiniMock, loop()).WillByDefault(InvokeWithoutArgs(m_pDfMiniMsgTimeout, &SimpleTimer::timerTick)); //called twice before timeout

    m_pMp3Control->play_prompt(prompt);
}

TEST_F(Mp3PlayTest, playPrompt_noSkip_playing_noTimeout)
{
    VoicePrompt prompt;
    prompt.allowSkip = false;
    prompt.promptId = MSG_CONFIRMED;
    // timeout not elapsing
    EXPECT_CALL(*m_pPinCtrl, digital_read(_)).Times(3).WillOnce(Return(false)).WillOnce(Return(false)).WillRepeatedly(Return(true)); // not playing
    EXPECT_CALL(m_dfMiniMock, loop()).Times(1);                                                                                      //called once before isplaying returns true

    m_pMp3Control->play_prompt(prompt);
}

TEST_F(Mp3PlayTest, get_trackCount_noFolder_Returns0)
{
    EXPECT_CALL(m_dfMiniMock, getFolderTrackCount(1)).WillRepeatedly(Return(0));
    EXPECT_EQ(0, m_pMp3Control->get_trackCount_of_folder(1));
}

TEST_F(Mp3PlayTest, get_trackCount_Folder_ReturnsNumber)
{
    Folder testFolder(1, Folder::ALBUM);
    testFolder.setTrackCount(8);
    EXPECT_CALL(m_dfMiniMock, getFolderTrackCount(1)).WillRepeatedly(Return(8));
    EXPECT_EQ(8, m_pMp3Control->get_trackCount_of_folder(1));
}

TEST_F(Mp3PlayTest, get_trackCount_TrackNumberTooHigh_Returns0)
{
    Folder testFolder(1, Folder::ALBUM);
    testFolder.setTrackCount(8);
    EXPECT_CALL(m_dfMiniMock, getFolderTrackCount(1)).WillRepeatedly(Return(256));
    EXPECT_EQ(0, m_pMp3Control->get_trackCount_of_folder(1));
}
#endif