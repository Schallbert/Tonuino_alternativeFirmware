#include "interface_UserInput.h"
#include "ClickEncoder.h"

// USERINPUT___CLICKENCODER ---------------------------------------------------------------
UserInput_ClickEncoder::UserInput_ClickEncoder() {} // : public UserInput(){}
UserInput_ClickEncoder::~UserInput_ClickEncoder() {}

void UserInput_ClickEncoder::set_input_pins(uint8_t pinA, uint8_t pinB, uint8_t pinSwitch)
{
    // pinA, pinB, pinSwitch are the pins of the encoder that are connected to the uC.
    this->pinA = pinA;
    this->pinB = pinB;
    this->pinSwitch = pinSwitch;
}

void UserInput_ClickEncoder::override_parameters()
{
// This function is for compile-time override of parameter standard values.
// encStepsPerNotch is the resolution of the encoder (datasheet).
// switchActiveState is the uC input level the switch shall be detected as pressed.
// doubleClickTime is the time interval [ms] in which two clicks must be detected to count as doubleclick event.
#define ENCSTEPS 4
#define ACTIVESTATE false
#define DOUBLECLICKTIME 400
    encStepsPerNotch = ENCSTEPS;
    switchActiveState = ACTIVESTATE;
    doubleClickTime = DOUBLECLICKTIME;
}

void UserInput_ClickEncoder::init()
{
    if (!(pinA && pinB && pinSwitch))
    {
        //Error: Need valid arduino input pins for encoder pins A, B, and switch!
        return;
    }

    //Call constructor of encoder class
    encoder = new ClickEncoder(pinA, pinB, pinSwitch, encStepsPerNotch, switchActiveState);
    //set defaults
    encoderPosition = 0;
    encoder->setAccelerationEnabled(false);
    encoder->setDoubleClickTime(doubleClickTime);
}

void UserInput_ClickEncoder::userinput_service_isr()
{
    encoder->service();
}

UserInput::UserRequest_e UserInput_ClickEncoder::get_user_request(bool cardDetected)
{
    if (UserInput::userInputLocked)
    {
        return NoAction;
    }

    //Poll for current encoder position and button state
    encoderPosition += encoder->getValue();
    buttonState = encoder->getButton();
    if (buttonState == ClickEncoder::Clicked)
    {
        return PlayPause;
    }

    if (get_encoder_diff() > 0)
    {
        //encoder turned right
        if (buttonState == ClickEncoder::Pressed || buttonState == ClickEncoder::Held)
        {
            return IncVolume; //while button was pressed/held: volume up
        }

        if (cardDetected)
        {
            return NextTrack; //no button press: next track
        }
    }

    if (get_encoder_diff() < 0)
    {
        //encoder turned left
        if (buttonState == ClickEncoder::Pressed || buttonState == ClickEncoder::Held)
        {
            return DecVolume; //while button was pressed/held: volume up
        }

        if (cardDetected)
        {
            return PrevTrack; //no button press: next track
        }
    }

    if (buttonState == ClickEncoder::Held)
    {
        // Button held but not turned
        if (cardDetected)
        {
            return Abort;
        }
        else
        {
            return Help;
        }
    }

    if (buttonState == ClickEncoder::DoubleClicked)
    {
        if (cardDetected)
        {
            UserInput::userInputLocked = !UserInput::userInputLocked; // Doubleclick and card present: lock buttons
        }
        else
        {
            return DelCard; // Doubleclick without card: delete card
        }
    }

    return NoAction;
}

int16_t UserInput_ClickEncoder::get_encoder_diff()
{
    static int16_t oldEncPos = 0;
    int16_t encDiff = 0;
    if (encoderPosition != oldEncPos)
    {
        encDiff = encoderPosition - oldEncPos;
        oldEncPos = encoderPosition;
    }
    return encDiff;
}
// USERINPUT___CLICKENCODER ---------------------------------------------------------------

// USERINPUT___3BUTTONS     ---------------------------------------------------------------
//UserInput_3Buttons::UserInput_3Buttons(){}

// Constructor: DigitalButton_SupportsLongPress
UserInput_3Buttons::DigitalButton_SupportsLongPress::DigitalButton_SupportsLongPress(
    int8_t pinId, bool active) : DigitalButton{pinId, active}, longPressTime(800), longPressRepeatInterval(400)
{
}

void UserInput_3Buttons::set_input_pins(uint8_t pinPlayPauseAbort, uint8_t pinPrev, uint8_t pinNext)
{
    // pinPrev, pinePlayPause, pinNext are the pins of the buttons that are connected to the uC.
    this->pinPrev = pinPrev;
    this->pinPlayPauseAbort = pinPlayPauseAbort;
    this->pinNext = pinNext;
}

void UserInput_3Buttons::override_parameters()
{
// longPressTime and longPressRepeatInterval are how long a switch must be pressed to detect a longPress event
// and if longPressed, what the repeat time should be to trigger events.
// switchActiveState is the uC input level the switch shall be detected as pressed.
// doubleClickTime is the time interval [ms] in which two clicks must be detected to count as doubleclick event.
#define ACTIVESTATE false
#define DOUBLECLICKTIME 400
#define LONGPRESSTIME 800
#define LONGPRESSREPEAT 400
    switchActiveState = ACTIVESTATE;
    doubleClickTime = DOUBLECLICKTIME;
    longPressTime = LONGPRESSTIME;
    longPressRepeatInterval = LONGPRESSREPEAT;
}

void UserInput_3Buttons::init()
{
    if (!(pinPrev && pinNext && pinPlayPauseAbort))
    {
        //Error: Need valid arduino input pi ns for encoder pins A, B, and switch!
        return;
    }

    // Call constructors of button class
    prevButton = new DigitalButton_SupportsLongPress(pinPrev, switchActiveState);
    plpsButton = new DigitalButton_SupportsLongPress(pinPlayPauseAbort, switchActiveState);
    nextButton = new DigitalButton_SupportsLongPress(pinNext, switchActiveState);
    //Set defaults
    prevButton->setDoubleClickTime(doubleClickTime);
    plpsButton->setDoubleClickTime(doubleClickTime);
    nextButton->setDoubleClickTime(doubleClickTime);
}

void UserInput_3Buttons::userinput_service_isr()
{
    prevButton->service();
    plpsButton->service();
    nextButton->service();
}

UserInput::UserRequest_e UserInput_3Buttons::get_user_request(bool cardDetected)
{
    if (UserInput::userInputLocked)
    {
        return NoAction;
    }

    //Get current button's states
    ClickEncoder::Button plpsBtnState = plpsButton->getButton();
    ClickEncoder::Button nextBtnState = nextButton->getButton();
    ClickEncoder::Button prevBtnState = prevButton->getButton();

    // --- PlayPauAbort button handler -----------------------------
    if (plpsBtnState == ClickEncoder::Clicked)
    {
        return PlayPause;
    }
    else if (plpsBtnState == ClickEncoder::Held)
    {
        // Button held
        if (cardDetected)
        {
            return Abort;
        }
        else
        {
            return Help;
        }
    }
    else if (plpsBtnState == ClickEncoder::DoubleClicked)
    {
        if (cardDetected)
        {
            // Doubleclick and card present: lock buttons
            UserInput::userInputLocked = !UserInput::userInputLocked;
        }
        else
        {
            // Doubleclick without card: delete card
            return DelCard;
        }
    }
    // --- Next button handler --------------------------------------
    if ((nextBtnState == ClickEncoder::Clicked) && cardDetected)
    {
        return NextTrack;
    }

    if (nextBtnState == ClickEncoder::Held)
    {
        nextButton->set_long_press_active(true);
        if (nextButton->handle_repeat_long_pressed())
        {
            return IncVolume;
        }
    }
    else
    {
        nextButton->set_long_press_active(false);
    }
    // --- Previous button handler ----------------------------------
    if ((prevBtnState == ClickEncoder::Clicked) && cardDetected)
    {
        return PrevTrack;
    }

    if (prevBtnState == ClickEncoder::Held)
    {
        prevButton->set_long_press_active(true);
        if (prevButton->handle_repeat_long_pressed())
        {
            return IncVolume;
        }
    }
    else
    {
        prevButton->set_long_press_active(false);
    }

    return NoAction;
}

void UserInput_3Buttons::DigitalButton_SupportsLongPress::service(void)
{
    DigitalButton::service();
    if (longPressActive)
    {
        longPressCount++;
    }
    else
    {
        longPressCount = 0;
    }
}

void UserInput_3Buttons::DigitalButton_SupportsLongPress::set_long_press_active(bool longPressActive)
{
    this->longPressActive = longPressActive;
}

bool UserInput_3Buttons::DigitalButton_SupportsLongPress::handle_repeat_long_pressed()
{
    if (longPressCount > longPressRepeatInterval)
    {
        longPressCount = 0;
        return true;
    }
    else
    {
        return false;
    }
}
// USERINPUT___3BUTTONS     ---------------------------------------------------------------

// USERINPUT___FACTORY      ---------------------------------------------------------------

UserInput *UserInput_Factory::getInstance(UserInputType_e typeKey)
{
    UserInput *pObject = NULL;

    switch (typeKey)
    {
    case Encoder:
        pObject = new UserInput_ClickEncoder;
        break;
    case ThreeButtons:
        pObject = new UserInput_3Buttons;
        break;
    default:
        pObject = new UserInput_3Buttons;
    }
    return pObject;
}
// USERINPUT___FACTORY      ---------------------------------------------------------------