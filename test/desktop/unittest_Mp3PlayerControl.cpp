#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../Mp3/Mp3Control/Mp3Control_implementation.h"
#include "../Utilities/SimpleTimer.h"

#include "mocks/unittest_Mp3Play_mocks.h"
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_ErrorHandler_mocks.h"

using ::testing::_;
using ::testing::InvokeWithoutArgs;
using ::testing::NiceMock;
using ::testing::Return;

// Fixture
class Mp3ControlTest : public ::testing::Test
{
protected:
    // Arrange
    virtual void SetUp()
    {
        m_pMp3Control = new Mp3Control(&m_dfMiniMock,
                                       &m_mp3PlayMock,
                                       &m_lullabyeTimer,
                                       &m_errorHandlerMock);
    }

    virtual void TearDown()
    {
        delete m_pMp3Control;
    }

protected:
    NiceMock<Mock_DfMiniMp3> m_dfMiniMock{};
    NiceMock<Mock_Mp3Play> m_mp3PlayMock{};
    SimpleTimer m_lullabyeTimer{};
    NiceMock<Mock_ErrorHandler> m_errorHandlerMock{};

    Mp3Control *m_pMp3Control{nullptr};
};

TEST_F(Mp3ControlTest, ClassConstructorMethodsCalled)
{
    EXPECT_CALL(m_dfMiniMock, setVolume(VOLUME_INIT));
    Mp3Control myMp3(&m_dfMiniMock, &m_mp3PlayMock, &m_lullabyeTimer, &m_errorHandlerMock);
}

TEST_F(Mp3ControlTest, AutoPlayCalledOnLoop)
{
    EXPECT_CALL(m_dfMiniMock, checkTrackFinished()).WillOnce(Return(false));
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, volumeUp_belowMax_volumeIsIncreased)
{
    EXPECT_CALL(m_dfMiniMock, getVolume()).WillRepeatedly(Return(VOLUME_MAX - 1));
    EXPECT_CALL(m_dfMiniMock, increaseVolume()).Times(1);
    m_pMp3Control->volume_up();
}

TEST_F(Mp3ControlTest, volumeUp_Max_volumeNotIncreased)
{
    EXPECT_CALL(m_dfMiniMock, getVolume()).WillRepeatedly(Return(VOLUME_MAX));
    EXPECT_CALL(m_dfMiniMock, increaseVolume()).Times(0); // not allowed to increase volume here
    m_pMp3Control->volume_up();
}

TEST_F(Mp3ControlTest, volumeDown_aboveMin_volumeIsDecreased)
{
    EXPECT_CALL(m_dfMiniMock, getVolume()).WillRepeatedly(Return(VOLUME_MIN + 1));
    EXPECT_CALL(m_dfMiniMock, decreaseVolume()).Times(1);
    m_pMp3Control->volume_down();
}

TEST_F(Mp3ControlTest, volumeDown_Min_volumeNotDecreased)
{
    EXPECT_CALL(m_dfMiniMock, getVolume()).WillRepeatedly(Return(VOLUME_MIN));
    EXPECT_CALL(m_dfMiniMock, decreaseVolume()).Times(0); // not allowed to increase volume here
    m_pMp3Control->volume_down();
}

TEST_F(Mp3ControlTest, play_Plays)
{
    EXPECT_CALL(m_dfMiniMock, start());
    m_pMp3Control->play();
}

TEST_F(Mp3ControlTest, pause_Pauses)
{
    EXPECT_CALL(m_dfMiniMock, pause());
    m_pMp3Control->pause();
}

TEST_F(Mp3ControlTest, autoplay_trackPlaying_nop)
{
    EXPECT_CALL(m_dfMiniMock, checkTrackFinished()).WillOnce(Return(false));

    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _)).Times(0);
    EXPECT_CALL(m_dfMiniMock, stop()).Times(0);

    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, autoplay_ALBUM_trackFinished_next)
{
    Folder testFolder(1, Folder::ALBUM);
    testFolder.setTrackCount(8);
    testFolder.is_valid(); // creates track queue, folder is valid
    ON_CALL(m_mp3PlayMock, getCurrentFolder()).WillByDefault(Return(testFolder));
    ON_CALL(m_dfMiniMock, checkTrackFinished()).WillByDefault(Return(true));

    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, 2)); // autoplay calls next_track.
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, autoplay_ONELARGETRACK_trackFinished_stop)
{
    Folder testFolder(1, Folder::ONELARGETRACK);
    testFolder.setTrackCount(8);

    ON_CALL(m_mp3PlayMock, getCurrentFolder()).WillByDefault(Return(testFolder));
    ON_CALL(m_dfMiniMock, checkTrackFinished()).WillByDefault(Return(true));

    EXPECT_CALL(m_dfMiniMock, stop()); // autoplay calls stop
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, autoplay_LULLABYE_trackFinished_next)
{
    Folder testFolder(1, Folder::LULLABYE);
    testFolder.setTrackCount(8);
    testFolder.is_valid(); // creates trackQueue, folder is valid.

    ON_CALL(m_mp3PlayMock, getCurrentFolder()).WillByDefault(Return(testFolder));
    ON_CALL(m_dfMiniMock, checkTrackFinished()).WillByDefault(Return(true));

    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, 2)); // autoplay calls next_track.
    EXPECT_CALL(m_dfMiniMock, stop()).Times(0);

    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, autoplay_LULLABYE_trackFinished_borderline_next)
{
    Folder testFolder(1, Folder::LULLABYE);
    testFolder.setTrackCount(8);
    testFolder.is_valid(); // creates trackQueue, folder is valid.

    ON_CALL(m_mp3PlayMock, getCurrentFolder()).WillByDefault(Return(testFolder));
    ON_CALL(m_dfMiniMock, checkTrackFinished()).WillByDefault(Return(true));

    m_lullabyeTimer.start(LULLABYE_TIMEOUT_SECS);
    // make timeout expire
    for (long i = 0; i < (LULLABYE_TIMEOUT_SECS - 1); ++i)
    {
        m_lullabyeTimer.timerTick();
    }

    EXPECT_CALL(m_dfMiniMock, stop()).Times(0);
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, autoplay_LULLABYE_trackFinished_timeout_stop)
{
    Folder testFolder(1, Folder::LULLABYE);
    testFolder.setTrackCount(8);
    testFolder.is_valid(); // creates trackQueue, folder is valid.

    ON_CALL(m_mp3PlayMock, getCurrentFolder()).WillByDefault(Return(testFolder));
    ON_CALL(m_dfMiniMock, checkTrackFinished()).WillByDefault(Return(true));
    // make timeout expire
    m_lullabyeTimer.start(LULLABYE_TIMEOUT_SECS);
    for (int i = 0; i < (LULLABYE_TIMEOUT_SECS); ++i)
    {
        m_lullabyeTimer.timerTick();
    }

    EXPECT_CALL(m_dfMiniMock, stop()); // autoplay calls stop
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, nextTrack_folderInvalid_setsError)
{
    Folder testFolder;
    ON_CALL(m_mp3PlayMock, getCurrentFolder()).WillByDefault(Return(testFolder));
    // trackCount not set
    EXPECT_CALL(m_errorHandlerMock, setFolderError());
    m_pMp3Control->next_track();
}

TEST_F(Mp3ControlTest, nextTrack_folderInvalid_noop)
{
    Folder testFolder;
    ON_CALL(m_mp3PlayMock, getCurrentFolder()).WillByDefault(Return(testFolder));
    // No folder defined!
    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _)).Times(0);
    m_pMp3Control->next_track();
}

TEST_F(Mp3ControlTest, nextTrack_trackCountNotSet_noop)
{
    Folder testFolder(1, Folder::ALBUM);
    ON_CALL(m_mp3PlayMock, getCurrentFolder()).WillByDefault(Return(testFolder));
    // trackCount not set
    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _)).Times(0);
    m_pMp3Control->next_track();
}

TEST_F(Mp3ControlTest, nextTrack_FolderSAVEPROGRESS_dependencyNotSet_noop)
{
    Folder testFolder(1, Folder::SAVEPROGRESS);
    testFolder.setTrackCount(8);
    ON_CALL(m_mp3PlayMock, getCurrentFolder()).WillByDefault(Return(testFolder));
    // dependencies not set
    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _)).Times(0);
    m_pMp3Control->next_track();
} 

TEST_F(Mp3ControlTest, nextTrack_FolderRANDOM_dependencyNotSet_noop)
{
    Folder testFolder(1, Folder::RANDOM);
    testFolder.setTrackCount(8);
    ON_CALL(m_mp3PlayMock, getCurrentFolder()).WillByDefault(Return(testFolder));
    // dependencies not set
    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _)).Times(0);
    m_pMp3Control->next_track();
}

TEST_F(Mp3ControlTest, nextTrack_FolderALBUM_dependencyNotSet_playsNext)
{
    Folder testFolder(1, Folder::ALBUM);
    testFolder.setTrackCount(8);
    ON_CALL(m_mp3PlayMock, getCurrentFolder()).WillByDefault(Return(testFolder));
    // dependencies not set
    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _));
    m_pMp3Control->next_track();
}

TEST_F(Mp3ControlTest, prevTrack_noFolder_noop)
{
    // No folder defined!
    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _)).Times(0);
    m_pMp3Control->prev_track();
}

TEST_F(Mp3ControlTest, prevTrack_FolderALBUM_dependencyNotSet_playsPrev)
{
    Folder testFolder(1, Folder::ALBUM);
    testFolder.setTrackCount(8);
    ON_CALL(m_mp3PlayMock, getCurrentFolder()).WillByDefault(Return(testFolder));
    // dependencies not set
    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _));
    m_pMp3Control->prev_track();
}

TEST_F(Mp3ControlTest, prevTrack_FolderALBUM_dependencySet_playsPrev)
{
    Folder testFolder(1, Folder::ALBUM);
    testFolder.setTrackCount(8);
    ON_CALL(m_mp3PlayMock, getCurrentFolder()).WillByDefault(Return(testFolder));

    EXPECT_CALL(m_dfMiniMock, playFolderTrack(_, _));
    m_pMp3Control->prev_track();
}