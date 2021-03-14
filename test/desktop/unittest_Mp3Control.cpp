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
    virtual void SetUp()
    {
        m_pMp3Control = new Mp3Control{m_dfMiniMock,
                            m_mp3PlayMock,
                            m_mp3PromptMock,
                            m_powerManagerMock,
                            m_messageHandlerMock};
    }

    virtual void TearDown()
    {
        delete m_pMp3Control;
        m_pMp3Control = nullptr;
    }

    NiceMock<Mock_DfMiniMp3> m_dfMiniMock{};
    NiceMock<Mock_Mp3Play> m_mp3PlayMock{};
    NiceMock<Mock_Mp3Prompt> m_mp3PromptMock{};
    NiceMock<Mock_PowerManager> m_powerManagerMock{};
    NiceMock<Mock_MessageHandler> m_messageHandlerMock{};

    Mp3Control* m_pMp3Control{nullptr};
};

TEST_F(Mp3ControlTest, loop_callsAutoplay)
{
    EXPECT_CALL(m_mp3PlayMock, autoplay());
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, loop_notifiesPowerManagerAboutIsPlaying)
{
    bool notPlaying{false};

    EXPECT_CALL(m_dfMiniMock, isPlaying()).WillOnce(Return(notPlaying));
    EXPECT_CALL(m_powerManagerMock, setPlayback(notPlaying));
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, loop_Mp3PlayerInit_prints3PlayerStatus)
{
    EXPECT_CALL(m_dfMiniMock, printStatus());
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, volumeUp_belowMax_volumeIsIncreased)
{
    ON_CALL(m_dfMiniMock, getVolume()).WillByDefault(Return(VOLUME_MAX - 1));

    EXPECT_CALL(m_dfMiniMock, increaseVolume()).Times(1);
    m_pMp3Control->setUserInput(Message::INPUTNEXTLP);
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, volumeUp_Max_volumeNotIncreased)
{
    ON_CALL(m_dfMiniMock, getVolume()).WillByDefault(Return(VOLUME_MAX));

    EXPECT_CALL(m_dfMiniMock, increaseVolume()).Times(0);
    m_pMp3Control->setUserInput(Message::INPUTNEXTLP);
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, volumeDown_aboveMin_volumeIsDecreased)
{
    ON_CALL(m_dfMiniMock, getVolume()).WillByDefault(Return(VOLUME_MIN + 1));

    EXPECT_CALL(m_dfMiniMock, decreaseVolume());
    m_pMp3Control->setUserInput(Message::INPUTPREVLP);
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, volumeDown_Min_volumeNotDecreased)
{
    ON_CALL(m_dfMiniMock, getVolume()).WillByDefault(Return(VOLUME_MIN));

    EXPECT_CALL(m_dfMiniMock, decreaseVolume()).Times(0);
    m_pMp3Control->setUserInput(Message::INPUTPREVLP);
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, prev_PrevTrack)
{
    m_pMp3Control->setUserInput(Message::INPUTPREV);

    EXPECT_CALL(m_mp3PlayMock, playPrev());
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, next_NextTrack)
{
    m_pMp3Control->setUserInput(Message::INPUTNEXT);

    EXPECT_CALL(m_mp3PlayMock, playNext());
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, help_promptsHelp)
{
    m_pMp3Control->setUserInput(Message::INPUTPLPSLP);

    EXPECT_CALL(m_mp3PromptMock, playPrompt(_));
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, pause_toggle_Plays)
{
    m_pMp3Control->setUserInput(Message::INPUTPLPS);
    ON_CALL(m_dfMiniMock, isPlaying()).WillByDefault(Return(false));

    EXPECT_CALL(m_dfMiniMock, start());
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, play_toggle_Pauses)
{
    m_pMp3Control->setUserInput(Message::INPUTPLPS);
    ON_CALL(m_dfMiniMock, isPlaying()).WillByDefault(Return(true));

    EXPECT_CALL(m_dfMiniMock, pause());
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, plpsDoubleClick_buttonsNotLocked_locksButtons)
{
    // lock input
    m_pMp3Control->setUserInput(Message::INPUTPLPSDC);
    m_pMp3Control->loop();
    // shouldn't allow nextTrack to be played
    m_pMp3Control->setUserInput(Message::INPUTNEXT);
    EXPECT_CALL(m_mp3PlayMock, playNext()).Times(0);
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, plpsDoubleClick_buttonsLocked_unlocksButtons)
{
    // lock input
    m_pMp3Control->setUserInput(Message::INPUTPLPSDC);
    m_pMp3Control->loop();
    //unlock
    m_pMp3Control->setUserInput(Message::INPUTPLPSDC);
    m_pMp3Control->loop();
    // should allow nextTrack to be played
    m_pMp3Control->setUserInput(Message::INPUTNEXT);
    EXPECT_CALL(m_mp3PlayMock, playNext());
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, plpsDoubleClick_buttonsLocked_playsLockPrompt)
{
    VoicePrompt expect{VoicePrompt::MSG_BUTTONLOCK, VoicePrompt::RESUMEPLAYBACK};
    m_pMp3Control->setUserInput(Message::INPUTPLPSDC);

    EXPECT_CALL(m_mp3PromptMock, playPrompt(expect));
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, plpsDoubleClick_buttonsUnlocked_playsUnlockPrompt)
{
    VoicePrompt expect{VoicePrompt::MSG_BUTTONFREE, VoicePrompt::RESUMEPLAYBACK};
    m_pMp3Control->setUserInput(Message::INPUTPLPSDC);
    m_pMp3Control->loop();

    m_pMp3Control->setUserInput(Message::INPUTPLPSDC);
    EXPECT_CALL(m_mp3PromptMock, playPrompt(expect));
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, plpsDoubleClick_trackCurrentlyPaused_continuesPlayback)
{
    m_pMp3Control->setUserInput(Message::INPUTPLPSDC);
    ON_CALL(m_dfMiniMock, isPlaying()).WillByDefault(Return(false));

    EXPECT_CALL(m_dfMiniMock, start());
    m_pMp3Control->loop();
}