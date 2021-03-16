#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Tonuino_config.h"
#include "SimpleTimer/SimpleTimer.h"
#include "Mp3Prompt/Mp3Prompt_implementation.h"

#include "mocks/unittest_DfMiniMp3_mocks.h"


using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::InvokeWithoutArgs;

class Mp3PromptTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pMp3Prompt = new Mp3Prompt(m_dfMiniMp3Mock,
                                     m_messageTimer);
    }

    virtual void TearDown()
    {
        delete m_pMp3Prompt;
        m_pMp3Prompt = nullptr;
    }

    NiceMock<Mock_DfMiniMp3> m_dfMiniMp3Mock{};
    SimpleTimer m_messageTimer{};

    Mp3Prompt *m_pMp3Prompt{nullptr};
};

TEST_F(Mp3PromptTest, PromptMessage_Undefined_WillNotPrompt)
{
    VoicePrompt undefined;
    EXPECT_CALL(m_dfMiniMp3Mock, playPrompt(_)).Times(0);
    m_pMp3Prompt->playPrompt(undefined);
}

TEST_F(Mp3PromptTest, PromptMessage_noSkipNotPlaying_Timeout)
{
    VoicePrompt prompt{VoicePrompt(VoicePrompt::MSG_HELP, VoicePrompt::PROMPT_ALLOWSKIP)};

    ON_CALL(m_dfMiniMp3Mock, isPlaying()).WillByDefault(Return(false)); // not playing
    ON_CALL(m_dfMiniMp3Mock, loop()).WillByDefault(InvokeWithoutArgs(&m_messageTimer, &SimpleTimer::timerTick));

    EXPECT_CALL(m_dfMiniMp3Mock, loop()).Times(WAIT_DFMINI_READY); // timeout kicks in. to wait system calls MP3's loop
    m_pMp3Prompt->playPrompt(prompt);
}

TEST_F(Mp3PromptTest, PromptMessage_noSkipNotFinishing_Timeout)
{
    VoicePrompt prompt{VoicePrompt(VoicePrompt::MSG_ABORTED, VoicePrompt::PROMPT_NOSKIP)};

    ON_CALL(m_dfMiniMp3Mock, isPlaying()).WillByDefault(Return(true));
    ON_CALL(m_dfMiniMp3Mock, loop()).WillByDefault(InvokeWithoutArgs(&m_messageTimer, &SimpleTimer::timerTick));

    EXPECT_CALL(m_dfMiniMp3Mock, loop()).Times(TIMEOUT_PROMPT_PLAYED); // timeout kicks in. to wait system calls MP3's loop
    m_pMp3Prompt->playPrompt(prompt);
}

TEST_F(Mp3PromptTest, PromptMessage_noSkipPlaying_onlyStartTimeout)
{
    VoicePrompt prompt{VoicePrompt(VoicePrompt::MSG_STARTUP, VoicePrompt::PROMPT_NOSKIP)};
    // timeout not elapsing
    EXPECT_CALL(m_dfMiniMp3Mock, isPlaying())
        .Times(3)
        .WillOnce(Return(true))                    // Called by WaitForPromptToStart()
        .WillOnce(Return(true))                    // All following(s) called by WaitForPromptToFinish();
        .WillRepeatedly(Return(false));            // Finishing before timeout
    EXPECT_CALL(m_dfMiniMp3Mock, loop()).Times(1); //called once before isplaying returns true
    m_pMp3Prompt->playPrompt(prompt);
}

TEST_F(Mp3PromptTest, PromptMessage_playStarts_willCallPrompt)
{
    VoicePrompt prompt{VoicePrompt(VoicePrompt::MSG_HELP, VoicePrompt::PROMPT_ALLOWSKIP)};

    ON_CALL(m_dfMiniMp3Mock, isPlaying()).WillByDefault(Return(true));
    EXPECT_CALL(m_dfMiniMp3Mock, playPrompt(_));
    m_pMp3Prompt->playPrompt(prompt);
}

TEST_F(Mp3PromptTest, PromptMessage_callTwice_wontPlayAgain)
{
    VoicePrompt prompt{VoicePrompt(VoicePrompt::MSG_ABORTED, VoicePrompt::PROMPT_ALLOWSKIP)};

    ON_CALL(m_dfMiniMp3Mock, isPlaying()).WillByDefault(Return(true));
    EXPECT_CALL(m_dfMiniMp3Mock, playPrompt(_)).Times(1);
    m_pMp3Prompt->playPrompt(prompt);
    m_pMp3Prompt->playPrompt(prompt);
}