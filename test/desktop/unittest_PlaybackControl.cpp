#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../OutputManager/PlaybackControl.h"

#include "mocks/unittest_Mp3Control_mocks.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// TEST FIXTURE
class PlaybackControlTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pPlaybackControl = new PlaybackControl(&m_mp3PlayerControlMock);
    }

    virtual void TearDown()
    {
        delete m_pPlaybackControl;
    }

protected:
    NiceMock<Mock_Mp3Control> m_mp3PlayerControlMock{};

    PlaybackControl *m_pPlaybackControl{nullptr};
};

TEST_F(PlaybackControlTest, playPause_togglesPlayPause)
{
    EXPECT_CALL(m_mp3PlayerControlMock, togglePlayPause());
    m_pPlaybackControl->handleUserInput(UserInput::PLAY_PAUSE);
}

TEST_F(PlaybackControlTest, next_callsNextTrack)
{
    EXPECT_CALL(m_mp3PlayerControlMock, nextTrack());
    m_pPlaybackControl->handleUserInput(UserInput::NEXT_TRACK);
}

TEST_F(PlaybackControlTest, incV_callsVolumeUp)
{
    EXPECT_CALL(m_mp3PlayerControlMock, volumeUp());
    m_pPlaybackControl->handleUserInput(UserInput::INC_VOLUME);
}

TEST_F(PlaybackControlTest, decV_callsVolumeDown)
{
    EXPECT_CALL(m_mp3PlayerControlMock, volumeDown());
    m_pPlaybackControl->handleUserInput(UserInput::DEC_VOLUME);
}

TEST_F(PlaybackControlTest, help_playsHelpPrompt)
{
    EXPECT_CALL(m_mp3PlayerControlMock, help());
    m_pPlaybackControl->handleUserInput(UserInput::PP_LONGPRESS);
}