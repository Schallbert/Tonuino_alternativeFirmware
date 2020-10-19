#include "UserInput_implementation.h"

void UserInput_ClickEncoder::userinput_service_isr()
{
    m_pEncoder->service();
}

UserInput::UserRequest_e UserInput_ClickEncoder::get_user_request()
{
    //Poll for current encoder position and button state
    userinput_refresh();

    if (buttonState == ClickEncoder_interface::DoubleClicked)
    {
        UserInput::userInputLocked = !UserInput::userInputLocked; // Doubleclick: lock buttons
    }

    if (UserInput::userInputLocked)
    {
        return NO_ACTION;
    }

    if (buttonState == ClickEncoder_interface::Clicked)
    {
        return PLAY_PAUSE;
    }

    if (encoderDiff > 0) // TODO: Check in INTEGRATION TEST if this really is a good idea. Alternative: take position.
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

    return NO_ACTION;
}

void UserInput_ClickEncoder::userinput_refresh()
{

    //Get values from encoder
    encoderDiff = m_pEncoder->getValue(); // diff to last "getValue" call
    encoderPosition += encoderDiff;
    buttonState = m_pEncoder->getButton();
}

// USERINPUT___CLICKENCODER ---------------------------------------------------------------

// USERINPUT___3BUTTONS     ---------------------------------------------------------------
//UserInput_3Buttons::UserInput_3Buttons(){}

void UserInput_3Buttons::userinput_service_isr()
{
    m_pPlpsButton->service();
    m_pNextButton->service();
    m_pPrevButton->service();
}

UserInput::UserRequest_e UserInput_3Buttons::get_user_request()
{
    //Get current button's states
    userinput_refresh();

    if (buttonStates.plpsButton == Encoder_longPressRepeat::DoubleClicked)
    {
        UserInput::userInputLocked = !UserInput::userInputLocked;
    }

    if (UserInput::userInputLocked)
    {
        return NO_ACTION;
    }

    if (buttonStates.plpsButton == Encoder_longPressRepeat::Clicked)
    {
        return PLAY_PAUSE;
    }
    else if (buttonStates.plpsButton == Encoder_longPressRepeat::Held ||
             buttonStates.plpsButton == Encoder_longPressRepeat::LongPressRepeat)
    {
        return PP_LONGPRESS;
    }

    // --- Next button handler --------------------------------------
    if (buttonStates.nextButton == Encoder_longPressRepeat::Clicked)
    {
        return NEXT_TRACK;
    }
    else if (buttonStates.nextButton == Encoder_longPressRepeat::LongPressRepeat)
    {
        return INC_VOLUME;
    }

    // --- Previous button handler ----------------------------------
    if (buttonStates.prevButton == Encoder_longPressRepeat::Clicked)
    {
        return PREV_TRACK;
    }
    else if (buttonStates.prevButton == Encoder_longPressRepeat::LongPressRepeat)
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