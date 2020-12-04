#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_ClickEncoder_mocks.h"

#include "../UserInput/ClickEncoder/ClickEncoder_supportsLongPress.h"
#include "../UserInput/UserInput/UserInput_implementation.h"

using ::testing::NiceMock;
using ::testing::_;
using ::testing::Return;
using ::testing::ReturnPointee;

class UserInput_3ButtonsTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pUserInput = new UserInput_3Buttons(&plPs, &next, &prev, longPressRepeatTicks);
    }

    virtual void TearDown()
    {
        delete m_pUserInput;
    }

protected:
    NiceMock<Mock_ClickEncoder> plPs{};
    NiceMock<Mock_ClickEncoder> next{};
    NiceMock<Mock_ClickEncoder> prev{};
    uint16_t longPressRepeatTicks{6}; 
    UserInput_3Buttons *m_pUserInput{nullptr};
};

class UserInput_ClickEncoderTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pUserInput = new UserInput_ClickEncoder(&enc);
    }

    virtual void TearDown()
    {
        delete m_pUserInput;
    }

protected:
    NiceMock<Mock_ClickEncoder> enc{};
    UserInput_ClickEncoder *m_pUserInput{nullptr};
};

TEST_F(UserInput_ClickEncoderTest, ServiceCalled)
{
    EXPECT_CALL(enc, service());

    m_pUserInput->userinput_service_isr();
}

TEST_F(UserInput_ClickEncoderTest, NoActionOnAnyButton_willReturnNoAction)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Open));
   
    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
}

TEST_F(UserInput_ClickEncoderTest, LockUserInput_buttonPress_willReturnNoAction)
{
    ClickEncoder_interface::eButtonState btnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(enc, getButton()).WillByDefault(ReturnPointee(&btnState)); 

    EXPECT_EQ(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
    btnState = ClickEncoder_interface::Clicked;
    EXPECT_EQ(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
}

TEST_F(UserInput_ClickEncoderTest, LockUserInput_turnRight_willReturnNoAction)
{
    ClickEncoder_interface::eButtonState btnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(enc, getButton()).WillByDefault(ReturnPointee(&btnState));
    ON_CALL(enc, getValue()).WillByDefault(Return(1));  

    EXPECT_EQ(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
    btnState = ClickEncoder_interface::Open;
    EXPECT_EQ(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
}

TEST_F(UserInput_ClickEncoderTest, LockUserInput_turnLeft_willReturnNoAction)
{
    ClickEncoder_interface::eButtonState btnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(enc, getButton()).WillByDefault(ReturnPointee(&btnState));
    ON_CALL(enc, getValue()).WillByDefault(Return(-1));  

    EXPECT_EQ(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
    btnState = ClickEncoder_interface::Open;
    EXPECT_EQ(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
}

TEST_F(UserInput_ClickEncoderTest, LockUserInput_Unlock_willReturnAction)
{
    ClickEncoder_interface::eButtonState btnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(enc, getButton()).WillByDefault(ReturnPointee(&btnState)); 
    m_pUserInput->get_user_request(); // lock
    m_pUserInput->get_user_request(); // unlock
    btnState = ClickEncoder_interface::Clicked;

    ASSERT_NE(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
}

TEST_F(UserInput_ClickEncoderTest, encClicked_willReturnPlayPause)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Clicked));

    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::PLAY_PAUSE);
}

TEST_F(UserInput_ClickEncoderTest, plpsHeld_willReturnPPLongPress)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));

    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::PP_LONGPRESS);
}

TEST_F(UserInput_ClickEncoderTest, turnRight_willReturnNextTrack)
{
    ON_CALL(enc, getValue()).WillByDefault(Return(1));

    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::NEXT_TRACK);
}

TEST_F(UserInput_ClickEncoderTest, turnRightWhileHeld_willReturnIncVolume)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));
    ON_CALL(enc, getValue()).WillByDefault(Return(1));

    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::INC_VOLUME);
}

TEST_F(UserInput_ClickEncoderTest, turnRightWhilePressed_willReturnIncVolume)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Pressed));
    ON_CALL(enc, getValue()).WillByDefault(Return(1));

    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::INC_VOLUME);
}

TEST_F(UserInput_ClickEncoderTest, turnLeft_willReturnPrevTrack)
{
    ON_CALL(enc, getValue()).WillByDefault(Return(-1));

    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::PREV_TRACK);
}

TEST_F(UserInput_ClickEncoderTest, turnLeftWhileHeld_willReturnDecVolume)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));
    ON_CALL(enc, getValue()).WillByDefault(Return(-1));

    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::DEC_VOLUME);
}

TEST_F(UserInput_ClickEncoderTest, turnLeftWhilePressed_willReturnDecVolume)
{
    ON_CALL(enc, getButton()).WillByDefault(Return(ClickEncoder_interface::Pressed));
    ON_CALL(enc, getValue()).WillByDefault(Return(-1));

    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::DEC_VOLUME);
}


TEST_F(UserInput_3ButtonsTest, ServiceCalled)
{
    EXPECT_CALL(plPs, service());
    EXPECT_CALL(next, service());
    EXPECT_CALL(prev, service());

    m_pUserInput->userinput_service_isr();
}

TEST_F(UserInput_3ButtonsTest, NoActionOnAnyButton_willReturnNoAction)
{
    ON_CALL(plPs, getButton()).WillByDefault(Return(ClickEncoder_interface::Open));
    ON_CALL(next, getButton()).WillByDefault(Return(ClickEncoder_interface::Open));
    ON_CALL(prev, getButton()).WillByDefault(Return(ClickEncoder_interface::Open));

    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
}

TEST_F(UserInput_3ButtonsTest, LockUserInput_plPsButtonPress_willReturnNoAction)
{
    ClickEncoder_interface::eButtonState plpsBtnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(plPs, getButton()).WillByDefault(ReturnPointee(&plpsBtnState)); 

    EXPECT_EQ(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
    plpsBtnState = ClickEncoder_interface::Clicked;
    EXPECT_EQ(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
}

TEST_F(UserInput_3ButtonsTest, LockUserInput_nextButtonPress_willReturnNoAction)
{
    ClickEncoder_interface::eButtonState plpsBtnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(plPs, getButton()).WillByDefault(ReturnPointee(&plpsBtnState));
    ON_CALL(next, getButton()).WillByDefault(Return(ClickEncoder_interface::Clicked));  

    EXPECT_EQ(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
    plpsBtnState = ClickEncoder_interface::Open;
    EXPECT_EQ(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
}

TEST_F(UserInput_3ButtonsTest, LockUserInput_prevButtonPress_willReturnNoAction)
{
    ClickEncoder_interface::eButtonState plpsBtnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(plPs, getButton()).WillByDefault(ReturnPointee(&plpsBtnState));
    ON_CALL(prev, getButton()).WillByDefault(Return(ClickEncoder_interface::Clicked));  

    EXPECT_EQ(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
    plpsBtnState = ClickEncoder_interface::Open;
    EXPECT_EQ(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
}

TEST_F(UserInput_3ButtonsTest, LockUserInput_Unlock_willReturnAction)
{
    ClickEncoder_interface::eButtonState plpsBtnState = ClickEncoder_interface::DoubleClicked;
    ON_CALL(plPs, getButton()).WillByDefault(ReturnPointee(&plpsBtnState)); 
    m_pUserInput->get_user_request(); // lock
    m_pUserInput->get_user_request(); // unlock
    plpsBtnState = ClickEncoder_interface::Clicked;

    ASSERT_NE(m_pUserInput->get_user_request(), UserInput::NO_ACTION);
}

TEST_F(UserInput_3ButtonsTest, plpsClicked_willReturnPlayPause)
{
    ON_CALL(plPs, getButton()).WillByDefault(Return(ClickEncoder_interface::Clicked));

    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::PLAY_PAUSE);
}

TEST_F(UserInput_3ButtonsTest, plpsHeld_willReturnPPLongPress)
{
    ON_CALL(plPs, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));

    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::PP_LONGPRESS);
}

TEST_F(UserInput_3ButtonsTest, plpsLongPressRepeat_willReturnPPLongPress)
{
    ON_CALL(plPs, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));
    for(uint8_t i = 0; i < longPressRepeatTicks + 1; ++i)
    {
        m_pUserInput->userinput_service_isr();
    }
    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::PP_LONGPRESS);
}

TEST_F(UserInput_3ButtonsTest, nextClicked_willReturnNextTrack)
{
    ON_CALL(next, getButton()).WillByDefault(Return(ClickEncoder_interface::Clicked));

    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::NEXT_TRACK);
}

TEST_F(UserInput_3ButtonsTest, nextLongPressRepeat_willReturnIncVolume)
{
    ON_CALL(next, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));
    for(uint8_t i = 0; i < longPressRepeatTicks + 1; ++i)
    {
        m_pUserInput->userinput_service_isr();
    }
    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::INC_VOLUME);
}

TEST_F(UserInput_3ButtonsTest, prevClicked_willReturnPrevTrack)
{
    ON_CALL(prev, getButton()).WillByDefault(Return(ClickEncoder_interface::Clicked));

    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::PREV_TRACK);
}

TEST_F(UserInput_3ButtonsTest, prevLongPressRepeat_willReturnDecVolume)
{
    ON_CALL(prev, getButton()).WillByDefault(Return(ClickEncoder_interface::Held));
    for(uint8_t i = 0; i < longPressRepeatTicks + 1; ++i)
    {
        m_pUserInput->userinput_service_isr();
    }
    ASSERT_EQ(m_pUserInput->get_user_request(), UserInput::DEC_VOLUME);
}

