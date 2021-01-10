#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_ArduinoIf_mocks.h"
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_MessageHandler_mocks.h"

#include "Tonuino_config.h"
#include "SimpleTimer/SimpleTimer.h"
#include "../MessageHandler/MessageHandler_implementation.h"

using ::testing::_;
using ::testing::InvokeWithoutArgs;
using ::testing::NiceMock;
using ::testing::Return;

// Fixture
class MessageHandlerTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pMessageHandler = new MessageHandler(m_serialMock,
                                              m_messagesMock,
                                              m_dfMiniMp3Mock,
                                              m_messageTimer);
    }

    virtual void TearDown()
    {
        delete m_pMessageHandler;
        m_pMessageHandler = nullptr;
    }

    NiceMock<Mock_serial> m_serialMock{};
    NiceMock<Mock_DfMiniMp3> m_dfMiniMp3Mock{};
    NiceMock<Mock_MessageToString> m_messagesMock{};
    SimpleTimer m_messageTimer{};

    MessageHandler *m_pMessageHandler{nullptr};
    char m_testString[5] = "test"; // 5 due to end character /0
};

// TODO: all tests
// PRINT MESSAGE ///////////////////////////////////////////////////////////////
TEST_F(MessageHandlerTest, PrintMessage_normal_willParseToString)
{
    EXPECT_CALL(m_messagesMock, getStringFromMessage(_));
    m_pMessageHandler->printMessage(Message{Message::STARTUP});
}

TEST_F(MessageHandlerTest, PrintMessage_normal_willPrint)
{
    ON_CALL(m_messagesMock, getStringFromMessage(_)).WillByDefault(Return(m_testString));
    EXPECT_CALL(m_serialMock, com_println(_));
    m_pMessageHandler->printMessage(Message{Message::STARTUP});
}

TEST_F(MessageHandlerTest, PrintMessage_sameMessagetwice_wontPrintAgain)
{
    ON_CALL(m_messagesMock, getStringFromMessage(_)).WillByDefault(Return(m_testString));
    EXPECT_CALL(m_serialMock, com_println(_)).Times(1);
    const Message testMessage{Message(Message::STARTUP)};
    m_pMessageHandler->printMessage(testMessage);
    m_pMessageHandler->printMessage(testMessage);
}

TEST_F(MessageHandlerTest, PrintMessage_differentMessages_willPrintBoth)
{
    ON_CALL(m_messagesMock, getStringFromMessage(_)).WillByDefault(Return(m_testString));
    EXPECT_CALL(m_serialMock, com_println(_)).Times(2);
    const Message testMessage{Message(Message::PAUSE)};
    const Message testMessage2{Message(Message::PLAY)};
    m_pMessageHandler->printMessage(testMessage);
    m_pMessageHandler->printMessage(testMessage2);
}

TEST_F(MessageHandlerTest, PrintMessage_MessagesofDifferentGroups_willPrintBoth)
{
    ON_CALL(m_messagesMock, getStringFromMessage(_)).WillByDefault(Return(m_testString));
    EXPECT_CALL(m_serialMock, com_println(_)).Times(2);
    const Message testMessage{Message(Message::SYSTEM, 3)};
    const Message testMessage2{Message(Message::NFCREADER, 3)};
    m_pMessageHandler->printMessage(testMessage);
    m_pMessageHandler->printMessage(testMessage2);
}

TEST_F(MessageHandlerTest, PrintMessage_offset_WillPrint)
{
    ON_CALL(m_messagesMock, getStringFromMessage(_)).WillByDefault(Return(m_testString));
    EXPECT_CALL(m_serialMock, com_println(_));
    m_pMessageHandler->printMessage(Message{Message::MP3PLAYBACK, 2});
}

TEST_F(MessageHandlerTest, PrintMessage_offset_ParsesCorrectMessage)
{
    const Message testOffset{Message(Message::MP3PLAYBACK, 2)};
    EXPECT_CALL(m_messagesMock, getStringFromMessage(identicalMessage(testOffset)));
    m_pMessageHandler->printMessage(testOffset);
}

// PROMPT MESSAGE ////////////////////////////////////////////////////////////
TEST_F(MessageHandlerTest, PromptMessage_Undefined_WillNotPrompt)
{
    VoicePrompt undefined;
    EXPECT_CALL(m_dfMiniMp3Mock, playMp3FolderTrack(_)).Times(0);
    m_pMessageHandler->promptMessage(undefined);
}

TEST_F(MessageHandlerTest, PromptMessage_noSkipNotPlaying_Timeout)
{
    VoicePrompt prompt;
    prompt.allowSkip = false;
    prompt.promptId = MSG_HELP;

    ON_CALL(m_dfMiniMp3Mock, isPlaying()).WillByDefault(Return(false)); // not playing
    ON_CALL(m_dfMiniMp3Mock, loop()).WillByDefault(InvokeWithoutArgs(&m_messageTimer, &SimpleTimer::timerTick));

    EXPECT_CALL(m_dfMiniMp3Mock, loop()).Times(WAIT_DFMINI_READY); // timeout kicks in. to wait system calls MP3's loop
    m_pMessageHandler->promptMessage(prompt);
}

TEST_F(MessageHandlerTest, PromptMessage_noSkipNotFinishing_Timeout)
{
    VoicePrompt prompt;
    prompt.allowSkip = false;
    prompt.promptId = MSG_ABORTED;

    ON_CALL(m_dfMiniMp3Mock, isPlaying()).WillByDefault(Return(true)); // not playing
    ON_CALL(m_dfMiniMp3Mock, loop()).WillByDefault(InvokeWithoutArgs(&m_messageTimer, &SimpleTimer::timerTick));

    EXPECT_CALL(m_dfMiniMp3Mock, loop()).Times(TIMEOUT_PROMPT_PLAYED); // timeout kicks in. to wait system calls MP3's loop
    m_pMessageHandler->promptMessage(prompt);
}

TEST_F(MessageHandlerTest, PromptMessage_noSkipPlaying_onlyStartTimeout)
{
    VoicePrompt prompt;
    prompt.allowSkip = false;
    prompt.promptId = MSG_STARTUP;
    // timeout not elapsing
    EXPECT_CALL(m_dfMiniMp3Mock, isPlaying())
        .Times(3)
        .WillOnce(Return(true))                    // Called by WaitForPromptToStart()
        .WillOnce(Return(true))                    // All following(s) called by WaitForPromptToFinish();
        .WillRepeatedly(Return(false));            // Finishing before timeout
    EXPECT_CALL(m_dfMiniMp3Mock, loop()).Times(1); //called once before isplaying returns true
    m_pMessageHandler->promptMessage(prompt);
}

TEST_F(MessageHandlerTest, PromptMessage_playStarts_willCallPrompt)
{
    VoicePrompt prompt;
    prompt.allowSkip = true;
    prompt.promptId = MSG_HELP;

    ON_CALL(m_dfMiniMp3Mock, isPlaying()).WillByDefault(Return(true));
    EXPECT_CALL(m_dfMiniMp3Mock, playMp3FolderTrack(_));
    m_pMessageHandler->promptMessage(prompt);
}

TEST_F(MessageHandlerTest, PromptMessage_callTwice_wontPlayAgain)
{
    VoicePrompt prompt;
    prompt.allowSkip = true;
    prompt.promptId = MSG_ABORTED;

    ON_CALL(m_dfMiniMp3Mock, isPlaying()).WillByDefault(Return(true));
    EXPECT_CALL(m_dfMiniMp3Mock, playMp3FolderTrack(_)).Times(1);
    m_pMessageHandler->promptMessage(prompt);
    m_pMessageHandler->promptMessage(prompt);
}