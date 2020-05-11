#include "unittest_UserInput.h"

namespace userinput
{


void userInput_3Buttons_pinsNotSet(void)
{
    UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);
    UserInput::UserRequest_e userAction = aUserInput->get_user_request(true);
    TEST_ASSERT_EQUAL(UserInput::UserRequest_e::Error, userAction);
}

void userInput_3Buttons_notInitialized(void)
{
    UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);
    aUserInput->set_input_pins(1, 2, 3);
    UserInput::UserRequest_e userAction = aUserInput->get_user_request(true);
    TEST_ASSERT_EQUAL(UserInput::UserRequest_e::Error, userAction);
}

void userInput_3Buttons_ready(void)
{
    UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);
    aUserInput->set_input_pins(1, 2, 3);
    aUserInput->init();
    UserInput::UserRequest_e userAction = aUserInput->get_user_request(true);
    TEST_ASSERT_EQUAL(UserInput::UserRequest_e::NoAction, userAction);
}

void userInput_3Buttons_noAction(void)
{
    UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);
    aUserInput->set_input_pins(1, 2, 3);
    aUserInput->init();
    aUserInput->userinput_service_isr();
    UserInput::UserRequest_e userAction = aUserInput->get_user_request(true);
    TEST_ASSERT_EQUAL(UserInput::UserRequest_e::NoAction, userAction);
}

void userInput_3Buttons_playPause(void)
{
    UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);
    aUserInput->set_input_pins(1, 2, 3);
    aUserInput->init();
    uint16_t i = 0;
    //logic is active low, so simulate LOW on button
    digitalWrite(1, LOW);
    while(i < 200) //simulate 200ms
    {
        i++;
        aUserInput->userinput_service_isr();
    }
    digitalWrite(1, HIGH); // rising edge: makes system detect "click event"
    aUserInput->userinput_service_isr();
    UserInput::UserRequest_e userAction = aUserInput->get_user_request(true);
    TEST_ASSERT_EQUAL(UserInput::UserRequest_e::PlayPause, userAction);
}




void userInput_encoder_pinsNotSet(void)
{
    UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::Encoder);
    UserInput::UserRequest_e userAction = aUserInput->get_user_request(true);
    TEST_ASSERT_EQUAL(UserInput::UserRequest_e::Error, userAction);
}

void userInput_encoder_notInitialized(void)
{
    UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::Encoder);
    aUserInput->set_input_pins(1, 2, 3);
    UserInput::UserRequest_e userAction = aUserInput->get_user_request(true);
    TEST_ASSERT_EQUAL(UserInput::UserRequest_e::Error, userAction);
}

void userInput_encoder_ready(void)
{
    UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::Encoder);
    aUserInput->set_input_pins(1, 2, 3);
    aUserInput->init();
    UserInput::UserRequest_e userAction = aUserInput->get_user_request(true);
    TEST_ASSERT_EQUAL(UserInput::UserRequest_e::NoAction, userAction);
}
/*
void userInput_factory_Encoder(void)
{
    UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::Encoder);
    
}*/

void unittests(void)
{
    RUN_TEST(userInput_3Buttons_pinsNotSet);
    RUN_TEST(userInput_3Buttons_notInitialized);
    RUN_TEST(userInput_3Buttons_ready);
    RUN_TEST(userInput_3Buttons_noAction);
    RUN_TEST(userInput_3Buttons_playPause);

    RUN_TEST(userInput_encoder_pinsNotSet);
    RUN_TEST(userInput_encoder_notInitialized);
    RUN_TEST(userInput_encoder_ready);
}
} // namespace userinput