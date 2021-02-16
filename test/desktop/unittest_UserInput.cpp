#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_ClickEncoder_mocks.h"
#include "mocks/unittest_Mp3Prompt_mocks.h"
#include "mocks/unittest_MessageHandler_mocks.h"

#include "../UserInput/UserInput/UserInput_implementation.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnPointee;

class UserInput_3ButtonsTest : public ::testing::Test
{
protected:
    NiceMock<Mock_ClickEncoder> m_plpsMock{};
    NiceMock<Mock_ClickEncoder> m_nextMock{};
    NiceMock<Mock_ClickEncoder> m_prevMock{};
    NiceMock<Mock_Mp3Prompt> m_Mp3PromptMock{};
    NiceMock<Mock_MessageHandler> m_MessageHandlerMock{};

    UserInput_3Buttons m_UserInput{m_plpsMock,
                                   m_nextMock,
                                   m_prevMock,
                                   m_Mp3PromptMock,
                                   m_MessageHandlerMock};
};

class UserInput_ClickEncoderTest : public ::testing::Test
{
protected:
    NiceMock<Mock_ClickEncoder> enc{};
    NiceMock<Mock_Mp3Prompt> m_Mp3PromptMock{};
    NiceMock<Mock_MessageHandler> m_MessageHandlerMock{};
    UserInput_ClickEncoder m_UserInput{enc,
                                       m_Mp3PromptMock,
                                       m_MessageHandlerMock};
};

TEST_F(UserInput_ClickEncoderTest, ServiceCalled)
{
    EXPECT_CALL(enc, service());

    m_UserInput.userinputServiceIsr();
}

TEST_F(UserInput_ClickEncoderTest, NoActionOnAnyButton_willReturnNoAction)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Open));

    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTNONE);
}

TEST_F(UserInput_ClickEncoderTest, LockUserInput_buttonPress_willReturnLocked)
{
    ClickEncoder_interface::eButtonState btnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(enc, getButton()).WillByDefault(ReturnPointee(&btnState));

    EXPECT_EQ(m_UserInput.getUserRequest(), Message::INPUTLOCK);
    btnState = ClickEncoder_interface::Clicked;
    EXPECT_EQ(m_UserInput.getUserRequest(), Message::INPUTLOCK);
}

TEST_F(UserInput_ClickEncoderTest, LockUserInput_turnRight_willReturnLocked)
{
    ClickEncoder_interface::eButtonState btnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(enc, getButton()).WillByDefault(ReturnPointee(&btnState));
    ON_CALL(enc, getIncrement()).WillByDefault(Return(1));

    EXPECT_EQ(m_UserInput.getUserRequest(), Message::INPUTLOCK);
    btnState = ClickEncoder_interface::Open;
    EXPECT_EQ(m_UserInput.getUserRequest(), Message::INPUTLOCK);
}

TEST_F(UserInput_ClickEncoderTest, LockUserInput_turnLeft_willReturnLocked)
{
    ClickEncoder_interface::eButtonState btnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(enc, getButton()).WillByDefault(ReturnPointee(&btnState));
    ON_CALL(enc, getIncrement()).WillByDefault(Return(-1));

    EXPECT_EQ(m_UserInput.getUserRequest(), Message::INPUTLOCK);
    btnState = ClickEncoder_interface::Open;
    EXPECT_EQ(m_UserInput.getUserRequest(), Message::INPUTLOCK);
}

TEST_F(UserInput_ClickEncoderTest, LockUserInput_Unlock_willReturnAction)
{
    ClickEncoder_interface::eButtonState btnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(enc, getButton()).WillByDefault(ReturnPointee(&btnState));
    m_UserInput.getUserRequest(); // lock
    m_UserInput.getUserRequest(); // unlock
    btnState = ClickEncoder_interface::Clicked;

    ASSERT_NE(m_UserInput.getUserRequest(), Message::INPUTNONE);
}

TEST_F(UserInput_ClickEncoderTest, encClicked_willReturnPlayPause)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Clicked));

    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTPLPS);
}

TEST_F(UserInput_ClickEncoderTest, plpsHeld_willReturnPPLongPress)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));

    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTPLPSLP);
}

TEST_F(UserInput_ClickEncoderTest, turnRight_willReturnNextTrack)
{
    ON_CALL(enc, getIncrement()).WillByDefault(Return(1));

    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTNEXT);
}

TEST_F(UserInput_ClickEncoderTest, turnRightWhileHeld_willReturnIncVolume)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));
    ON_CALL(enc, getIncrement()).WillByDefault(Return(1));

    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTNEXTLP);
}

TEST_F(UserInput_ClickEncoderTest, turnRightWhilePressed_willReturnIncVolume)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Closed));
    ON_CALL(enc, getIncrement()).WillByDefault(Return(1));

    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTNEXTLP);
}

TEST_F(UserInput_ClickEncoderTest, turnLeft_willReturnPrevTrack)
{
    ON_CALL(enc, getIncrement()).WillByDefault(Return(-1));

    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTPREV);
}

TEST_F(UserInput_ClickEncoderTest, turnLeftWhileHeld_willReturnDecVolume)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));
    ON_CALL(enc, getIncrement()).WillByDefault(Return(-1));

    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTPREVLP);
}

TEST_F(UserInput_ClickEncoderTest, turnLeftWhilePressed_willReturnDecVolume)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Closed));
    ON_CALL(enc, getIncrement()).WillByDefault(Return(-1));

    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTPREVLP);
}

TEST_F(UserInput_3ButtonsTest, ServiceCalled)
{
    EXPECT_CALL(m_plpsMock, service());
    EXPECT_CALL(m_nextMock, service());
    EXPECT_CALL(m_prevMock, service());

    m_UserInput.userinputServiceIsr();
}

TEST_F(UserInput_3ButtonsTest, NoActionOnAnyButton_willReturnNoAction)
{
    ON_CALL(m_plpsMock, getButton()).WillByDefault(Return(ClickEncoder_interface::Open));
    ON_CALL(m_nextMock, getButton()).WillByDefault(Return(ClickEncoder_interface::Open));
    ON_CALL(m_prevMock, getButton()).WillByDefault(Return(ClickEncoder_interface::Open));

    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTNONE);
}

TEST_F(UserInput_3ButtonsTest, LockUserInput_m_plpsMockButtonPress_willReturnLocked)
{
    ClickEncoder_interface::eButtonState plpsBtnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(m_plpsMock, getButton()).WillByDefault(ReturnPointee(&plpsBtnState));

    EXPECT_EQ(m_UserInput.getUserRequest(), Message::INPUTLOCK);
    plpsBtnState = ClickEncoder_interface::Clicked;
    EXPECT_EQ(m_UserInput.getUserRequest(), Message::INPUTLOCK);
}

TEST_F(UserInput_3ButtonsTest, LockUserInput_m_nextMockButtonPress_willReturnLocked)
{
    ClickEncoder_interface::eButtonState plpsBtnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(m_plpsMock, getButton()).WillByDefault(ReturnPointee(&plpsBtnState));
    ON_CALL(m_nextMock, getButton()).WillByDefault(Return(ClickEncoder_interface::Clicked));

    EXPECT_EQ(m_UserInput.getUserRequest(), Message::INPUTLOCK);
    plpsBtnState = ClickEncoder_interface::Open;
    EXPECT_EQ(m_UserInput.getUserRequest(), Message::INPUTLOCK);
}

TEST_F(UserInput_3ButtonsTest, LockUserInput_prevButtonPress_willReturnLocked)
{
    ClickEncoder_interface::eButtonState plpsBtnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(m_plpsMock, getButton()).WillByDefault(ReturnPointee(&plpsBtnState));
    ON_CALL(m_prevMock, getButton()).WillByDefault(Return(ClickEncoder_interface::Clicked));

    EXPECT_EQ(m_UserInput.getUserRequest(), Message::INPUTLOCK);
    plpsBtnState = ClickEncoder_interface::Open;
    EXPECT_EQ(m_UserInput.getUserRequest(), Message::INPUTLOCK);
}

TEST_F(UserInput_3ButtonsTest, LockUserInput_Unlock_willReturnAction)
{
    ClickEncoder_interface::eButtonState plpsBtnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(m_plpsMock, getButton()).WillByDefault(ReturnPointee(&plpsBtnState));
    m_UserInput.getUserRequest(); // lock
    m_UserInput.getUserRequest(); // unlock
    plpsBtnState = ClickEncoder_interface::Clicked;

    ASSERT_NE(m_UserInput.getUserRequest(), Message::INPUTLOCK);
}

TEST_F(UserInput_3ButtonsTest, plpsClicked_willReturnPlayPause)
{
    ON_CALL(m_plpsMock, getButton()).WillByDefault(Return(ClickEncoder_interface::Clicked));

    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTPLPS);
}

TEST_F(UserInput_3ButtonsTest, plpsHeld_willReturnPPLongPress)
{
    ON_CALL(m_plpsMock, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));

    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTPLPSLP);
}

TEST_F(UserInput_3ButtonsTest, plpsLongPressRepeat_willReturnPPLongPress)
{
    ON_CALL(m_plpsMock, getButton()).WillByDefault(Return(ClickEncoder_interface::LongPressRepeat));
    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTNONE);
}

TEST_F(UserInput_3ButtonsTest, m_nextMockClicked_willReturnNextTrack)
{
    ON_CALL(m_nextMock, getButton()).WillByDefault(Return(ClickEncoder_interface::Clicked));

    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTNEXT);
}

TEST_F(UserInput_3ButtonsTest, m_nextMockLongPressRepeat_willReturnIncVolume)
{
    ON_CALL(m_nextMock, getButton()).WillByDefault(Return(ClickEncoder_interface::LongPressRepeat));
    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTNEXTLP);
}

TEST_F(UserInput_3ButtonsTest, prevClicked_willReturnPrevTrack)
{
    ON_CALL(m_prevMock, getButton()).WillByDefault(Return(ClickEncoder_interface::Clicked));

    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTPREV);
}

TEST_F(UserInput_3ButtonsTest, prevLongPressRepeat_willReturnDecVolume)
{
    ON_CALL(m_prevMock, getButton()).WillByDefault(Return(ClickEncoder_interface::LongPressRepeat));
    ASSERT_EQ(m_UserInput.getUserRequest(), Message::INPUTPREVLP);
}
