#include "UserInput_implementation.h"

void UserInput_ClickEncoder::userinput_service_isr()
{
    m_pEncoder->service();
}

UserInput::UserRequest_e UserInput_ClickEncoder::get_user_request()
{
    if (UserInput::userInputLocked)
    {
        return NO_ACTION;
    }

    //Poll for current encoder position and button state
    userinput_refresh();

    if (buttonState == ClickEncoder_interface::Clicked)
    {
        return PLAY_PAUSE;
    }

    if (encoderDiff > 0)
    {
        //encoder turned right
        if (buttonState == ClickEncoder_interface::Pressed ||
            buttonState == ClickEncoder_interface::Held)
        {
            return INC_VOLUME; //while button was pressed/held: volume up
        }
        else
        {
            return NEXT_TRACK; //no button press: next track
        }
    }

    if (encoderDiff < 0)
    {
        //encoder turned left
        if (buttonState == ClickEncoder_interface::Pressed ||
            buttonState == ClickEncoder_interface::Held)
        {
            return DEC_VOLUME; //while button was pressed/held: volume up
        }
        else
        {
            return PREV_TRACK; //no button press: next track
        }
    }

    if (buttonState == ClickEncoder_interface::Held)
    {
        // Button held but not turned
        return PP_LONGPRESS;
    }

    if (buttonState == ClickEncoder_interface::DoubleClicked)
    {
        UserInput::userInputLocked = !UserInput::userInputLocked; // Doubleclick: lock buttons
    }

    return NO_ACTION;
}

void UserInput_ClickEncoder::userinput_refresh()
{
    static int16_t oldEncPos = 0;

    //Get values from encoder
    encoderPosition += m_pEncoder->getValue();
    buttonState = m_pEncoder->getButton();

    if (encoderPosition != oldEncPos)
    {
        encoderDiff = encoderPosition - oldEncPos;
        oldEncPos = encoderPosition;
    }
    else
    {
        encoderDiff = 0;
    }
}

// USERINPUT___CLICKENCODER ---------------------------------------------------------------

// USERINPUT___3BUTTONS     ---------------------------------------------------------------
//UserInput_3Buttons::UserInput_3Buttons(){}

void UserInput_3Buttons::userinput_service_isr()
{
    m_pPrevButton->service();
    m_pPlpsButton->service();
    m_pNextButton->service();
}

UserInput::UserRequest_e UserInput_3Buttons::get_user_request()
{
    if (UserInput::userInputLocked)
    {
        return NO_ACTION;
    }

    //Get current button's states
    userinput_refresh();

    // --- PlayPauAbort button handler -----------------------------
    if (buttonStates.plpsButton == Encoder_longPressRepeat::Clicked)
    {
        return PLAY_PAUSE;
    }
    else if (buttonStates.plpsButton == Encoder_longPressRepeat::Held ||
             buttonStates.nextButton == Encoder_longPressRepeat::LongPressRepeat)
    {
        // Button held
        return PP_LONGPRESS;
    }
    else if (buttonStates.plpsButton == Encoder_longPressRepeat::DoubleClicked)
    {
        UserInput::userInputLocked = !UserInput::userInputLocked;
    }
    // --- Next button handler --------------------------------------
    if (buttonStates.nextButton == Encoder_longPressRepeat::Clicked)
    {
        return NEXT_TRACK;
    }

    if (buttonStates.nextButton == Encoder_longPressRepeat::LongPressRepeat)
    {
        return INC_VOLUME;
    }

    // --- Previous button handler ----------------------------------
    if (buttonStates.prevButton == Encoder_longPressRepeat::Clicked)
    {
        return PREV_TRACK;
    }

    if (buttonStates.prevButton == Encoder_longPressRepeat::LongPressRepeat)
    {
        return DEC_VOLUME;
    }

    return NO_ACTION;
}

void UserInput_3Buttons::userinput_refresh()
{
    buttonStates.plpsButton = m_pPlpsButton->getButton();
    buttonStates.nextButton = m_pNextButton->getButton();
    buttonStates.prevButton = m_pPrevButton->getButton();
}

// USERINPUT___3BUTTONS     ---------------------------------------------------------------

/*
// USERINPUT___FACTORY      ---------------------------------------------------------------

UserInput *UserInput_Factory::getInstance(UserInputType_e typeKey)
{
    UserInput *pObject = NULL;

    switch (typeKey)
    {
    case ENCODER_1:
        pObject = new UserInput_ClickEncoder;
        break;
    case THREE_BUTTONS:
        pObject = new UserInput_3Buttons;
        break;
    default:
        pObject = new UserInput_3Buttons;
    }
    return pObject;
} */
// USERINPUT___FACTORY      ---------------------------------------------------------------