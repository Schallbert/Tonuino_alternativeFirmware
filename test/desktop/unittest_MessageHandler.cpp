#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_ArduinoDIcontainer_mocks.h"
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
        m_pMessageHandler = new MessageHandler(&m_ArduDIMock,
                                               &m_mp3PlayMock);
    }

    virtual void TearDown()
    {
        delete m_pMessageHandler;
    }

protected:
    NiceMock<Mock_ArduinoDIcontainer> m_ArduDIMock{};
    NiceMock<Mock_Mp3Play> m_mp3PlayMock{};

    MessageHandler *m_pMessageHandler{nullptr};
};

/*
TEST_F(MessageHandlerTest, ClassConstructorMethodsCalled)
{
    EXPECT_CALL(m_dfMiniMock, setVolume(VOLUME_INIT));
    Mp3Control myMp3(&m_dfMiniMock, &m_mp3PlayMock, &m_errorHandlerMock);
}*/