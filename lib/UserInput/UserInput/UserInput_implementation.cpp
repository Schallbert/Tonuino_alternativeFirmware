#include "UserInput_implementation.h"

void UserInput_ClickEncoder::init()
    {
        isInitiated = true;
        m_rMessageHandler.printMessage(Message::INPUTONLINE);
    }

void UserInput_ClickEncoder::userinputServiceIsr()
{
    m_Encoder.service();
}

Message::eMessageContent UserInput_ClickEncoder::getUserRequest()
{
    Message::eMessageContent result{Message::INPUTNONE};
    if (!isInitiated)
    {
        return result;
    }

    //Get values from encoder
    int16_t encoderDiff = m_Encoder.getIncrement(); // diff to last "getValue" call
    Button::eButtonStates buttonState = m_Encoder.getButton();

    if (buttonState == Button::DoubleClicked)
    {
        if (UserInput_interface::isLocked)
        {
            UserInput_interface::isLocked = false;
            VoicePrompt locked{VoicePrompt::MSG_BUTTONFREE, true};
            m_rPrompt.playPrompt(locked);
        }
        else
        {
            UserInput_interface::isLocked = true;
            VoicePrompt unlocked{VoicePrompt::MSG_BUTTONLOCK, true};
            m_rPrompt.playPrompt(unlocked);
        }
    }

    if (UserInput_interface::isLocked)
    {
        result = Message::INPUTLOCK;
    }
    else if (buttonState == Button::Clicked)
    {
        result = Message::INPUTPLPS;
    }
    else if (encoderDiff > 0) // TODO: Check in INTEGRATION TEST if this really is a good idea. Alternative: take position.
    {
        //encoder turned right
        if (buttonState == Button::Held || buttonState == Button::Closed)
        {
            result = Message::INPUTNEXTLP;
        }
        else
        {
            result = Message::INPUTNEXT;
        }
    }
    else if (encoderDiff < 0)
    {
        //encoder turned left
        if (buttonState == Button::Held || buttonState == Button::Closed)
        {
            result = Message::INPUTPREVLP;
        }
        else
        {
            result = Message::INPUTPREV;
        }
    }
    else if (buttonState == Button::Held) // Button not turned
    {
        result = Message::INPUTPLPSLP;
    }
    else
    {
        result = Message::INPUTNONE;
    }

    m_rMessageHandler.printMessage(result);
    return result;
}

// USERINPUT___CLICKENCODER ---------------------------------------------------------------

// USERINPUT___3BUTTONS     ---------------------------------------------------------------
//UserInput_3Buttons::UserInput_3Buttons(){}

void UserInput_3Buttons::init()
    {
        isInitiated = true;
        m_rMessageHandler.printMessage(Message::INPUTONLINE);
    }

void UserInput_3Buttons::userinputServiceIsr()
{
    m_PlpsButton.service();
    m_NextButton.service();
    m_PrevButton.service();
}

Message::eMessageContent UserInput_3Buttons::getUserRequest()
{
    Message::eMessageContent result{Message::INPUTNONE};
    if (!isInitiated)
    {
        return result;
    }
    
    //Get current button's states
    Button::eButtonStates plpsButtonState{m_PlpsButton.getButton()};
    Button::eButtonStates nextButtonState{m_NextButton.getButton()};
    Button::eButtonStates prevButtonState{m_PrevButton.getButton()};

    // TODO: ISSUE: SOMEHOW THE VOICEPROMT MAY NOT BE CALLED HERE (REF ISSUE?) IT WILL BREAK WHEN I DO THAT
    if (plpsButtonState == Button::DoubleClicked)
    {
        if (UserInput_interface::isLocked)
        {
            UserInput_interface::isLocked = false;
            VoicePrompt locked{VoicePrompt::MSG_BUTTONFREE, false};
            m_rPrompt.playPrompt(locked);
        }
        else
        {
            UserInput_interface::isLocked = true;
            VoicePrompt unlocked{VoicePrompt::MSG_BUTTONLOCK, false};
            m_rPrompt.playPrompt(unlocked);
        }
    }

    if (UserInput_interface::isLocked)
    {
        result = Message::INPUTLOCK;
    }
    else if (plpsButtonState == Button::Clicked)
    {
        result = Message::INPUTPLPS;
    }
    else if (plpsButtonState == Button::Held)
    {
        result = Message::INPUTPLPSLP;
    }
    else if (nextButtonState == Button::Clicked)
    {
        result = Message::INPUTNEXT;
    }
    else if (nextButtonState == Button::LongPressRepeat)
    {
        result = Message::INPUTNEXTLP;
    }
    else if (prevButtonState == Button::Clicked)
    {
        result = Message::INPUTPREV;
    }
    else if (prevButtonState == Button::LongPressRepeat)
    {
        result = Message::INPUTPREVLP;
    }
    else
    {
        result = Message::INPUTNONE;
    }

    m_rMessageHandler.printMessage(result);
    return result;
}

// USERINPUT___3BUTTONS     ---------------------------------------------------------------