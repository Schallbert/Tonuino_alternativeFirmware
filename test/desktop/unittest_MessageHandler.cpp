#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_ArduinoIf_mocks.h"
#include "mocks/unittest_Mp3Play_mocks.h"

#include "../MessageHandler/MessageHandler_implementation.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// Fixture
class MessageHandlerTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pMessageHandler = new MessageHandler(&m_serialMock,
                                               &m_mp3PlayMock);
    }

    virtual void TearDown()
    {
        delete m_pMessageHandler;
    }

protected:
    NiceMock<Mock_serial> m_serialMock{};
    NiceMock<Mock_Mp3Play> m_mp3PlayMock{};

    MessageHandler *m_pMessageHandler{nullptr};
};

TEST_F(MessageHandlerTest, PrintMessage_nullptr_WontPrint)
{
    EXPECT_CALL(m_serialMock, com_println(_)).Times(0);
    m_pMessageHandler->printMessage(nullptr);
}

TEST_F(MessageHandlerTest, PrintMessage_emptyString_WontPrint)
{
    EXPECT_CALL(m_serialMock, com_println(_)).Times(0);
    m_pMessageHandler->printMessage("");
}

TEST_F(MessageHandlerTest, PrintMessage_Valid_WillPrint)
{
    EXPECT_CALL(m_serialMock, com_println(_));
    m_pMessageHandler->printMessage("Test");
}

TEST_F(MessageHandlerTest, PromptMessage_Undefined_WillNotPrompt)
{
    VoicePrompt undefined;
    EXPECT_CALL(m_mp3PlayMock, playPrompt(_)).Times(0);
    m_pMessageHandler->promptMessage(undefined);
}

TEST_F(MessageHandlerTest, PromptMessage_New_WilltPrompt)
{
    VoicePrompt valid;
    valid.allowSkip = true;
    valid.promptId = MSG_HELP;
    EXPECT_CALL(m_mp3PlayMock, playPrompt(_));
    m_pMessageHandler->promptMessage(valid);
}

TEST_F(MessageHandlerTest, PromptMessage_NotNew_WillNotPrompt)
{
    VoicePrompt noNewContent;
    noNewContent.allowSkip = true;
    noNewContent.promptId = MSG_HELP;
    m_pMessageHandler->promptMessage(noNewContent);

    EXPECT_CALL(m_mp3PlayMock, playPrompt(_)).Times(0);
    m_pMessageHandler->promptMessage(noNewContent);
}