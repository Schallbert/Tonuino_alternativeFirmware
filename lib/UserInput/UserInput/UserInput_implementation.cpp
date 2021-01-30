#include "Tonuino_config.h"
#include "Arduino_config.h"

#include "UserInput_implementation.h"

void UserInput_ClickEncoder::userinputServiceIsr()
{
    m_rEncoder.service();
}

Message::eMessageContent UserInput_ClickEncoder::getUserRequest()
{
    //Poll for current encoder position and button state
    userinputRefresh();

    if (buttonState == ClickEncoder_interface::DoubleClicked)
    {
        if (UserInput_interface::userInputLocked)
        {
            UserInput_interface::userInputLocked = false;
            VoicePrompt locked{VoicePrompt::MSG_BUTTONFREE, true};
            m_rPrompt.playPrompt(locked);
        }
        else
        {
            UserInput_interface::userInputLocked = true;
            VoicePrompt unlocked{VoicePrompt::MSG_BUTTONLOCK, true};
            m_rPrompt.playPrompt(unlocked);
        }
    }

    if (UserInput_interface::userInputLocked)
    {
        return Message::INLOCK;
    }

    if (buttonState == ClickEncoder_interface::Clicked)
    {
        return Message::INPLPS;
    }

    if (encoderDiff > 0) // TODO: Check in INTEGRATION TEST if this really is a good idea. Alternative: take position.
    {
        //encoder turned right
        if (buttonState == ClickEncoder_interface::Pressed ||
            buttonState == ClickEncoder_interface::Held)
        {
            return Message::INNEXTLP;
        }
        else
        {
            return Message::INNEXT;
        }
    }

    if (encoderDiff < 0)
    {
        //encoder turned left
        if (buttonState == ClickEncoder_interface::Pressed ||
            buttonState == ClickEncoder_interface::Held)
        {
            return Message::INPREVLP;
        }
        else
        {
            return Message::INPREV;
        }
    }

    if (buttonState == ClickEncoder_interface::Held)
    {
        // Button held but not turned
        return Message::INPLPSLP;
    }

    return Message::INNONE;
}

void UserInput_ClickEncoder::userinputRefresh()
{

    //Get values from encoder
    encoderDiff = m_rEncoder.getValue(); // diff to last "getValue" call
    encoderPosition += encoderDiff;
    buttonState = m_rEncoder.getButton();
}

// USERINPUT___CLICKENCODER ---------------------------------------------------------------

// USERINPUT___3BUTTONS     ---------------------------------------------------------------
//UserInput_3Buttons::UserInput_3Buttons(){}

void UserInput_3Buttons::userinputServiceIsr()
{
    m_PlpsButton.service();
    m_NextButton.service();
    m_PrevButton.service();
}

Message::eMessageContent UserInput_3Buttons::getUserRequest()
{
    //Get current button's states
    userinputRefresh();

    if (buttonStates.plpsButton == Encoder_longPressRepeat::DoubleClicked)
    {
        if (UserInput_interface::userInputLocked)
        {
            UserInput_interface::userInputLocked = false;
            VoicePrompt locked{VoicePrompt::MSG_BUTTONFREE, true};
            m_rPrompt.playPrompt(locked);
        }
        else
        {
            UserInput_interface::userInputLocked = true;
            VoicePrompt unlocked{VoicePrompt::MSG_BUTTONLOCK, true};
            m_rPrompt.playPrompt(unlocked);
        }
    }

    if (UserInput_interface::userInputLocked)
    {
        return Message::INLOCK;
    }

    if (buttonStates.plpsButton == Encoder_longPressRepeat::Clicked)
    {
        return Message::INPLPS;
    }
    else if (buttonStates.plpsButton == Encoder_longPressRepeat::Held ||
             buttonStates.plpsButton == Encoder_longPressRepeat::LongPressRepeat)
    {
        return Message::INPLPSLP;
    }

    // --- Next button handler --------------------------------------
    if (buttonStates.nextButton == Encoder_longPressRepeat::Clicked)
    {
        return Message::INNEXT;
    }
    else if (buttonStates.nextButton == Encoder_longPressRepeat::LongPressRepeat)
    {
        return Message::INNEXTLP;
    }

    // --- Previous button handler ----------------------------------
    if (buttonStates.prevButton == Encoder_longPressRepeat::Clicked)
    {
       return Message::INPREV;
    }
    else if (buttonStates.prevButton == Encoder_longPressRepeat::LongPressRepeat)
    {
        return Message::INPREVLP;
    }

    return Message::INNONE;
}

void UserInput_3Buttons::userinputRefresh()
{
    buttonStates.plpsButton = m_PlpsButton.getButton();
    buttonStates.nextButton = m_NextButton.getButton();
    buttonStates.prevButton = m_PrevButton.getButton();
}

// USERINPUT___3BUTTONS     ---------------------------------------------------------------