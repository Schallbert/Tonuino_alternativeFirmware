#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_ClickEncoder_mocks.h"

#include "../UserInput/ClickEncoder_implementation/ClickEncoder_supportsLongPress.h"
#include "../UserInput/UserInput_implementation/UserInput_implementation.h"

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

