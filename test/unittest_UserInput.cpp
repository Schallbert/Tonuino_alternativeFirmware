#include "unittest_UserInput.h"

namespace userinput
{

    void userInput_3Buttons_pinsNotSet(void)
    {
        UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);
        UserInput::UserRequest_e userAction = aUserInput->get_user_request();
        TEST_ASSERT_EQUAL(UserInput::UserRequest_e::Error, userAction);
    }

    void userInput_3Buttons_notInitialized(void)
    {
        UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);
        aUserInput->set_input_pins(1, 2, 3);
        UserInput::UserRequest_e userAction = aUserInput->get_user_request();
        TEST_ASSERT_EQUAL(UserInput::UserRequest_e::Error, userAction);
    }

    void userInput_3Buttons_ready(void)
    {
        UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);
        aUserInput->set_input_pins(1, 2, 3);
        aUserInput->init();
        UserInput::UserRequest_e userAction = aUserInput->get_user_request();
        TEST_ASSERT_EQUAL(UserInput::UserRequest_e::NoAction, userAction);
    }

    void userInput_3Buttons_getUserRequest(void)
    {
        // This test is meant to check the output of get_user_request. It creates fake input to this function
        // and then listens if get_user_Requests returns correct answers.
        // Arrange
        UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);
        aUserInput->set_input_pins(1, 2, 3);
        aUserInput->init();
        // Act
        aUserInput->set_card_detected(true);
        for (int i = UserInput::UserRequest_e::NoAction; i != UserInput::UserRequest_e::Error; ++i)
        {
            aUserInput->set_fake_user_request(static_cast<UserInput::UserRequest_e>(i));
            // Assert
            TEST_ASSERT_EQUAL(i, aUserInput->get_user_request());
        }
    }

    void userinput_3Buttons_serviceIsr(void)
    {
        // Arrange
        uint16_t breakTheLoop = 0xFFFF;
        uint16_t i = 0;
        UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);
        aUserInput->set_input_pins(1, 2, 3);
        aUserInput->init();
        aUserInput->set_card_detected(true);
        // Act
        aUserInput->set_fake_user_request(UserInput::UserRequest_e::IncVolume);
        // test with longPress as a working service will invoke incVolume every LongPressRepeatInterval's cycle
        do 
        {
            i++;
            aUserInput->userinput_service_isr();

        } while((aUserInput->get_user_request() != UserInput::UserRequest_e::IncVolume) && (i < breakTheLoop));
        TEST_ASSERT_LESS_THAN(breakTheLoop, i);
        TEST_MESSAGE("Results shoud be euqal to LongPressRepeatInterval (default: 400 in constructor DigitalButton_SupportsLongPress:");
    } 

    void userInput_encoder_pinsNotSet(void)
    {
        UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::Encoder);
        UserInput::UserRequest_e userAction = aUserInput->get_user_request();
        TEST_ASSERT_EQUAL(UserInput::UserRequest_e::Error, userAction);
    }

    void userInput_encoder_notInitialized(void)
    {
        UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::Encoder);
        aUserInput->set_input_pins(1, 2, 3);
        UserInput::UserRequest_e userAction = aUserInput->get_user_request();
        TEST_ASSERT_EQUAL(UserInput::UserRequest_e::Error, userAction);
    }

    void userInput_encoder_ready(void)
    {
        UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::Encoder);
        aUserInput->set_input_pins(1, 2, 3);
        aUserInput->init();
        UserInput::UserRequest_e userAction = aUserInput->get_user_request();
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
        RUN_TEST(userInput_3Buttons_getUserRequest);
        RUN_TEST(userinput_3Buttons_serviceIsr);

        RUN_TEST(userInput_encoder_pinsNotSet);
        RUN_TEST(userInput_encoder_notInitialized);
        RUN_TEST(userInput_encoder_ready);
    }
} // namespace userinput