#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_ArduinoIf_mocks.h"
#include "mocks/unittest_MessageHandler_mocks.h"

#include "Tonuino_config.h"
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
        m_pMessageHandler = new MessageHandler(m_serialMock,
                                              m_messagesMock);
    }

    virtual void TearDown()
    {
        delete m_pMessageHandler;
        m_pMessageHandler = nullptr;
    }

    NiceMock<Mock_serial> m_serialMock{};
    NiceMock<Mock_MessageToString> m_messagesMock{};

    MessageHandler *m_pMessageHandler{nullptr};
    char m_testString[5] = "test"; // 5 due to end character /0
};

// PRINT MESSAGE ///////////////////////////////////////////////////////////////
TEST_F(MessageHandlerTest, PrintMessage_normal_willParseToString)
{
    EXPECT_CALL(m_messagesMock, getStringFromMessage(_));
    m_pMessageHandler->printMessage(Message::STARTUP);
}

TEST_F(MessageHandlerTest, PrintMessage_normal_willPrint)
{
    ON_CALL(m_messagesMock, getStringFromMessage(_)).WillByDefault(Return(m_testString));
    EXPECT_CALL(m_serialMock, com_println(_));
    m_pMessageHandler->printMessage(Message::STARTUP);
}

TEST_F(MessageHandlerTest, PrintMessage_sameMessagetwice_wontPrintAgain)
{
    ON_CALL(m_messagesMock, getStringFromMessage(_)).WillByDefault(Return(m_testString));
    EXPECT_CALL(m_serialMock, com_println(_)).Times(1);
    const Message::eMessageContent testMessage{Message::STARTUP};
    m_pMessageHandler->printMessage(testMessage);
    m_pMessageHandler->printMessage(testMessage);
}

TEST_F(MessageHandlerTest, PrintMessage_differentMessages_willPrintBoth)
{
    ON_CALL(m_messagesMock, getStringFromMessage(_)).WillByDefault(Return(m_testString));
    EXPECT_CALL(m_serialMock, com_println(_)).Times(2);
    m_pMessageHandler->printMessage(Message::PAUSE);
    m_pMessageHandler->printMessage(Message::PLAY);
}

TEST_F(MessageHandlerTest, PrintMessage_MessagesofDifferentGroups_willPrintBoth)
{
    ON_CALL(m_messagesMock, getStringFromMessage(_)).WillByDefault(Return(m_testString));
    EXPECT_CALL(m_serialMock, com_println(_)).Times(2);
    const Message testMessage{Message(Message::SYSTEM, 3)};
    const Message testMessage2{Message(Message::NFCREADER, 3)};
    m_pMessageHandler->printMessage(testMessage.getContent());
    m_pMessageHandler->printMessage(testMessage2.getContent());
}

TEST_F(MessageHandlerTest, PrintMessage_offset_WillPrint)
{
    ON_CALL(m_messagesMock, getStringFromMessage(_)).WillByDefault(Return(m_testString));
    EXPECT_CALL(m_serialMock, com_println(_));
    const Message testMessage{Message(Message::MP3PLAYBACK, 2)};
    m_pMessageHandler->printMessage(testMessage.getContent());
}

TEST_F(MessageHandlerTest, PrintMessage_offset_ParsesCorrectMessage)
{
    const Message testOffset{Message(Message::MP3PLAYBACK, 2)};
    EXPECT_CALL(m_messagesMock, getStringFromMessage(identicalMessage(testOffset.getContent())));
    m_pMessageHandler->printMessage(testOffset.getContent());
}