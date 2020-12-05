#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../Mp3/Mp3Play/Mp3Play_implementation.h"
#include "../Utilities/SimpleTimer/SimpleTimer.h"

#include "mocks/unittest_ArduinoDIcontainer_mocks.h"
#include "mocks/unittest_ArduinoIf_mocks.h"
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_MessageHandler_mocks.h"

using ::testing::_;
using ::testing::InvokeWithoutArgs;
using ::testing::NiceMock;
using ::testing::Return;

// Fixture
class Mp3Play : public ::testing::Test
{
protected:
    // Arrange
    virtual void SetUp()
    {
        m_arduinoHalMock.DelegateToMockPins(&pinControlMock);
        m_arduinoHalMock.DelegateToMockEeprom(&eepromMock);
        m_pMp3Play = new Mp3Play_implementation(&m_arduinoHalMock,
                                                &m_dfMiniMock,
                                                &m_lullabyeTimer,
                                                &m_dfMiniPromptTimer,
                                                &m_messageHandlerMock);
    }

    virtual void TearDown()
    {
        delete m_pMp3Play;
    }

protected:
    NiceMock<Mock_ArduinoDIcontainer> m_arduinoHalMock{};
    NiceMock<Mock_DfMiniMp3> m_dfMiniMock{};
    SimpleTimer m_dfMiniPromptTimer{};
    SimpleTimer m_lullabyeTimer{};
    NiceMock<Mock_MessageHandler> m_messageHandlerMock{};
    NiceMock<Mock_pinCtrl> pinControlMock{};
    NiceMock<Mock_eeprom> eepromMock{};

    Mp3Play_implementation *m_pMp3Play{nullptr};
};

// IS PLAYING ////////////////////////////////////////////////////////////
TEST_F(Mp3Play, isPlaying_notPlaying_returnsFalse)
{
    ON_CALL(pinControlMock, digital_read(_)).WillByDefault(Return(true));

    ASSERT_FALSE(m_pMp3Play->isPlaying());
}

TEST_F(Mp3Play, isPlaying_Playing_returnsTrue)
{
    ON_CALL(pinControlMock, digital_read(_)).WillByDefault(Return(false));

    ASSERT_TRUE(m_pMp3Play->isPlaying());
}

// PLAY FOLDER ////////////////////////////////////////////////////////////
TEST_F(Mp3Play, playFolder_notInitialized_setsFolderError)
{
    Folder unInitializedFolder;

    EXPECT_CALL(m_messageHandlerMock, promptMessage(_));
    m_pMp3Play->playFolder(unInitializedFolder);
}

TEST_F(Mp3Play, playFolder_playerCannotFindFolderOnSdCard_setsFolderError)
{
    Folder nonExistantFolder(1, Folder::ALBUM);
    ON_CALL(m_dfMiniMock, getFolderTrackCount(_)).WillByDefault(Return(0));

    EXPECT_CALL(m_messageHandlerMock, promptMessage(_));
    m_pMp3Play->playFolder(nonExistantFolder);
}

TEST_F(Mp3Play, playFolder_folderValid_playsFolder)
{
    Folder validFolder(1, Folder::ALBUM);
    ON_CALL(m_dfMiniMock, getFolderTrackCount(_)).WillByDefault(Return(1));

    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _));
    m_pMp3Play->playFolder(validFolder);
}

TEST_F(Mp3Play, playFolder_folderValid_notifiesPlaying)
{
    Folder validFolder(1, Folder::ALBUM);
    ON_CALL(m_dfMiniMock, getFolderTrackCount(_)).WillByDefault(Return(1));

    EXPECT_CALL(m_messageHandlerMock, printMessage(_)).Times(2); // once for PlayMode, once for playing
    m_pMp3Play->playFolder(validFolder);
}

TEST_F(Mp3Play, playFolder_callTwice_wontPlayAgain)
{
    Folder validFolder(1, Folder::ALBUM);
    ON_CALL(m_dfMiniMock, getFolderTrackCount(_)).WillByDefault(Return(1));

    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _)).Times(1);
    m_pMp3Play->playFolder(validFolder);
    m_pMp3Play->playFolder(validFolder);
}

// AUTOPLAY ////////////////////////////////////////////////////////////
TEST_F(Mp3Play, autoplay_trackPlaying_nop)
{
    ON_CALL(m_dfMiniMock, isTrackFinished()).WillByDefault(Return(false));

    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _)).Times(0);
    EXPECT_CALL(m_dfMiniMock, stop()).Times(0);

    m_pMp3Play->autoplay();
}

TEST_F(Mp3Play, autoplay_ALBUM_trackFinished_next)
{
    ON_CALL(m_dfMiniMock, getFolderTrackCount(_)).WillByDefault(Return(8));
    Folder testFolder(1, Folder::ALBUM);
    m_pMp3Play->playFolder(testFolder);

    ON_CALL(m_dfMiniMock, isTrackFinished()).WillByDefault(Return(true));

    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, 2)); // autoplay calls playNext.
    m_pMp3Play->autoplay();
}

TEST_F(Mp3Play, autoplay_ONELARGETRACK_trackFinished_stop)
{
    ON_CALL(m_dfMiniMock, getFolderTrackCount(_)).WillByDefault(Return(8));
    Folder testFolder(1, Folder::ONELARGETRACK);
    m_pMp3Play->playFolder(testFolder);

    ON_CALL(m_dfMiniMock, isTrackFinished()).WillByDefault(Return(true));

    EXPECT_CALL(m_dfMiniMock, stop()); // autoplay calls stop
    m_pMp3Play->autoplay();
}

TEST_F(Mp3Play, autoplay_LULLABYE_trackFinished_next)
{
    ON_CALL(m_dfMiniMock, getFolderTrackCount(_)).WillByDefault(Return(8));
    Folder testFolder(1, Folder::LULLABYE);
    m_pMp3Play->playFolder(testFolder);

    ON_CALL(m_dfMiniMock, isTrackFinished()).WillByDefault(Return(true));

    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, 2)); // autoplay calls playNext.
    EXPECT_CALL(m_dfMiniMock, stop()).Times(0);

    m_pMp3Play->autoplay();
}

TEST_F(Mp3Play, autoplay_LULLABYE_trackFinished_borderline_next)
{
    ON_CALL(m_dfMiniMock, getFolderTrackCount(_)).WillByDefault(Return(8));
    Folder testFolder(1, Folder::LULLABYE);
    m_pMp3Play->playFolder(testFolder);

    ON_CALL(m_dfMiniMock, isTrackFinished()).WillByDefault(Return(true));

    m_lullabyeTimer.start(LULLABYE_TIMEOUT_SECS);
    // make timeout expire
    for (long i = 0; i < (LULLABYE_TIMEOUT_SECS - 1); ++i)
    {
        m_lullabyeTimer.timerTick();
    }

    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, 2)); // autoplay calls playNext.
    m_pMp3Play->autoplay();
}

TEST_F(Mp3Play, autoplay_LULLABYE_trackFinished_timeout_stop)
{
    ON_CALL(m_dfMiniMock, getFolderTrackCount(_)).WillByDefault(Return(8));
    Folder testFolder(1, Folder::LULLABYE);
    m_pMp3Play->playFolder(testFolder);

    ON_CALL(m_dfMiniMock, isTrackFinished()).WillByDefault(Return(true));
    // make timeout expire
    m_lullabyeTimer.start(LULLABYE_TIMEOUT_SECS);
    for (int i = 0; i < (LULLABYE_TIMEOUT_SECS); ++i)
    {
        m_lullabyeTimer.timerTick();
    }

    EXPECT_CALL(m_dfMiniMock, stop()); // autoplay calls stop
    m_pMp3Play->autoplay();
}

// NEXT TRACK ////////////////////////////////////////////////////////////
TEST_F(Mp3Play, playNext_folderInvalid_setsError)
{
    Folder testFolder;
    m_pMp3Play->playFolder(testFolder);
    // trackCount not set
    EXPECT_CALL(m_messageHandlerMock, printMessage(_));
    m_pMp3Play->playNext();
}

TEST_F(Mp3Play, playNext_folderInvalid_noop)
{
    Folder testFolder;
    m_pMp3Play->playFolder(testFolder);
    // No folder defined!
    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _)).Times(0);
    m_pMp3Play->playNext();
}

TEST_F(Mp3Play, playNext_folderNotOnSdCard_noop)
{
    Folder testFolder(1, Folder::ALBUM);
    m_pMp3Play->playFolder(testFolder);
    // trackCount not set
    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _)).Times(0);
    m_pMp3Play->playNext();
}

TEST_F(Mp3Play, playNext_playsNext)
{
    ON_CALL(m_dfMiniMock, getFolderTrackCount(_)).WillByDefault(Return(8));
    Folder testFolder(1, Folder::SAVEPROGRESS);
    m_pMp3Play->playFolder(testFolder);
    // dependencies not set
    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _));
    m_pMp3Play->playNext();
}

// PREV TRACK ////////////////////////////////////////////////////////////
TEST_F(Mp3Play, playPrev_folderInvalid_setsError)
{
    Folder testFolder;
    m_pMp3Play->playFolder(testFolder);
    // trackCount not set
    EXPECT_CALL(m_messageHandlerMock, printMessage(_));
    m_pMp3Play->playPrev();
}

TEST_F(Mp3Play, playPrev_noFolder_noop)
{
    // No folder defined!
    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _)).Times(0);
    m_pMp3Play->playPrev();
}

TEST_F(Mp3Play, playPrev_folderNotOnSdCard_noop)
{
    Folder testFolder(1, Folder::ALBUM);
    m_pMp3Play->playFolder(testFolder);
    // trackCount not set
    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _)).Times(0);
    m_pMp3Play->playPrev();
}

TEST_F(Mp3Play, playPrev_playsPrev)
{
    ON_CALL(m_dfMiniMock, getFolderTrackCount(_)).WillByDefault(Return(8));
    Folder testFolder(1, Folder::SAVEPROGRESS);
    m_pMp3Play->playFolder(testFolder);
    // dependencies not set
    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _));
    m_pMp3Play->playPrev();
}

// PLAY PROMPT ////////////////////////////////////////////////////////////
TEST_F(Mp3Play, playPrompt_noSkipNotPlaying_Timeout)
{
    VoicePrompt prompt;
    prompt.allowSkip = false;
    prompt.promptId = MSG_CONFIRMED;

    ON_CALL(pinControlMock, digital_read(_)).WillByDefault(Return(true)); // not playing                                                               // not playing
    ON_CALL(m_dfMiniMock, loop()).WillByDefault(InvokeWithoutArgs(&m_dfMiniPromptTimer, &SimpleTimer::timerTick));

    EXPECT_CALL(m_dfMiniMock, loop()).Times(WAIT_DFMINI_READY); // timeout kicks in. to wait system calls MP3's loop
    m_pMp3Play->playPrompt(prompt);
}

TEST_F(Mp3Play, playPrompt_noSkipNotFinishing_Timeout)
{
    VoicePrompt prompt;
    prompt.allowSkip = false;
    prompt.promptId = MSG_ABORTED;

    ON_CALL(pinControlMock, digital_read(_)).WillByDefault(Return(false)); //playing                                                               // not playing
    ON_CALL(m_dfMiniMock, loop()).WillByDefault(InvokeWithoutArgs(&m_dfMiniPromptTimer, &SimpleTimer::timerTick));

    EXPECT_CALL(m_dfMiniMock, loop()).Times(TIMEOUT_PROMPT_PLAYED); // timeout kicks in. to wait system calls MP3's loop
    m_pMp3Play->playPrompt(prompt);
}

TEST_F(Mp3Play, playPrompt_noSkipPlaying_onlyStartTimeout)
{
    VoicePrompt prompt;
    prompt.allowSkip = false;
    prompt.promptId = MSG_CONFIRMED;
    // timeout not elapsing
    EXPECT_CALL(pinControlMock, digital_read(_))
        .Times(3)
        .WillOnce(Return(false))                                // playing
        .WillOnce(Return(false))                                // playing
        .WillRepeatedly(Return(true));                          // not playing
    EXPECT_CALL(m_dfMiniMock, loop()).Times(WAIT_DFMINI_READY); //called once before isplaying returns true
    m_pMp3Play->playPrompt(prompt);
}

TEST_F(Mp3Play, playPrompt_callTwice_wontPlayAgain)
{
    VoicePrompt prompt;
    prompt.allowSkip = true;
    prompt.promptId = MSG_ABORTED;

    EXPECT_CALL(m_dfMiniMock, playAdvertisement(_)).Times(1);
    m_pMp3Play->playPrompt(prompt);
    m_pMp3Play->playPrompt(prompt);
}