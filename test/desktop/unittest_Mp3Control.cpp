#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Tonuino_config.h"
#include "Folder.h"

#include "mocks/unittest_Mp3Play_mocks.h"
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_Mp3Prompt_mocks.h"
#include "mocks/unittest_PowerManager_Mocks.h"
#include "mocks/unittest_MessageHandler_mocks.h"

#include "../UserInput/UserInput/UserInput_interface.h"
#include "../Mp3/Mp3Control/Mp3Control_implementation.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// Fixture
class Mp3ControlTest : public ::testing::Test
{
protected:
    NiceMock<Mock_DfMiniMp3> m_dfMiniMock{};
    NiceMock<Mock_Mp3Play> m_mp3PlayMock{};
    NiceMock<Mock_Mp3Prompt> m_mp3PromptMock{};
    NiceMock<Mock_PowerManager> m_powerManagerMock{};
    NiceMock<Mock_MessageHandler> m_messageHandlerMock{};

    Mp3Control m_Mp3Control{m_dfMiniMock,
                            m_mp3PlayMock,
                            m_mp3PromptMock,
                            m_powerManagerMock,
                            m_messageHandlerMock};
};

TEST_F(Mp3ControlTest, loop_callsAutoplay)
{
    EXPECT_CALL(m_mp3PlayMock, autoplay());
    m_Mp3Control.loop();
}

TEST_F(Mp3ControlTest, loop_notifiesPowerManagerAboutIsPlaying)
{
    bool notPlaying{false};

    EXPECT_CALL(m_dfMiniMock, isPlaying()).WillOnce(Return(notPlaying));
    EXPECT_CALL(m_powerManagerMock, setPlayback(notPlaying));
    m_Mp3Control.loop();
}

TEST_F(Mp3ControlTest, loop_Mp3PlayerInit_prints3PlayerStatus)
{
    EXPECT_CALL(m_dfMiniMock, printStatus());
    m_Mp3Control.loop();
}

TEST_F(Mp3ControlTest, volumeUp_belowMax_volumeIsIncreased)
{
    ON_CALL(m_dfMiniMock, getVolume()).WillByDefault(Return(VOLUME_MAX - 1));

    EXPECT_CALL(m_dfMiniMock, increaseVolume()).Times(1);
    m_Mp3Control.setUserInput(Message::INPUTNEXTLP);
    m_Mp3Control.loop();
}

TEST_F(Mp3ControlTest, volumeUp_Max_volumeNotIncreased)
{
    ON_CALL(m_dfMiniMock, getVolume()).WillByDefault(Return(VOLUME_MAX));

    EXPECT_CALL(m_dfMiniMock, increaseVolume()).Times(0);
    m_Mp3Control.setUserInput(Message::INPUTNEXTLP);
    m_Mp3Control.loop();
}

TEST_F(Mp3ControlTest, volumeDown_aboveMin_volumeIsDecreased)
{
    ON_CALL(m_dfMiniMock, getVolume()).WillByDefault(Return(VOLUME_MIN + 1));

    EXPECT_CALL(m_dfMiniMock, decreaseVolume());
    m_Mp3Control.setUserInput(Message::INPUTPREVLP);
    m_Mp3Control.loop();
}

TEST_F(Mp3ControlTest, volumeDown_Min_volumeNotDecreased)
{
    ON_CALL(m_dfMiniMock, getVolume()).WillByDefault(Return(VOLUME_MIN));

    EXPECT_CALL(m_dfMiniMock, decreaseVolume()).Times(0);
    m_Mp3Control.setUserInput(Message::INPUTPREVLP);
    m_Mp3Control.loop();
}

TEST_F(Mp3ControlTest, prev_PrevTrack)
{
    m_Mp3Control.setUserInput(Message::INPUTPREV);

    EXPECT_CALL(m_mp3PlayMock, playPrev());
    m_Mp3Control.loop();
}

TEST_F(Mp3ControlTest, next_NextTrack)
{
    m_Mp3Control.setUserInput(Message::INPUTNEXT);

    EXPECT_CALL(m_mp3PlayMock, playNext());
    m_Mp3Control.loop();
}

TEST_F(Mp3ControlTest, help_promptsHelp)
{
    m_Mp3Control.setUserInput(Message::INPUTPLPSLP);

    EXPECT_CALL(m_mp3PromptMock, playPrompt(_));
    m_Mp3Control.loop();
}

TEST_F(Mp3ControlTest, pause_toggle_Plays)
{
    m_Mp3Control.setUserInput(Message::INPUTPLPS);
    ON_CALL(m_dfMiniMock, isPlaying()).WillByDefault(Return(false));

    EXPECT_CALL(m_dfMiniMock, start());
    m_Mp3Control.loop();
}

TEST_F(Mp3ControlTest, play_toggle_Pauses)
{
    m_Mp3Control.setUserInput(Message::INPUTPLPS);
    ON_CALL(m_dfMiniMock, isPlaying()).WillByDefault(Return(true));

    EXPECT_CALL(m_dfMiniMock, pause());
    m_Mp3Control.loop();
}