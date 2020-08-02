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
    state = inputPinsSet;
}

void UserInput_ClickEncoder::init()
{
    if (state != inputPinsSet)
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
    state = ready;
}

void UserInput_ClickEncoder::userinput_service_isr()
{
    if (state == ready)
    {
        encoder->service();
    }
}

UserInput::UserRequest_e UserInput_ClickEncoder::get_user_request()
{
    if (state != ready)
    {
        return Error;
    }
    else if (UserInput::userInputLocked)
    {
        return NO_ACTION;
    }

    //Poll for current encoder position and button state
    #ifndef UNIT_TEST
    userinput_refresh();
    #endif

    if (buttonState == ClickEncoder::Clicked)
    {
        return PLAY_PAUSE;
    }

    if (encoderDiff > 0)
    {
        //encoder turned right
        if (buttonState == ClickEncoder::Pressed || buttonState == ClickEncoder::Held)
        {
            return INC_VOLUME; //while button was pressed/held: volume up
        }

        if (cardDetected)
        {
            return NEXT_TRACK; //no button press: next track
        }
    }

    if (encoderDiff < 0)
    {
        //encoder turned left
        if (buttonState == ClickEncoder::Pressed || buttonState == ClickEncoder::Held)
        {
            return DEC_VOLUME; //while button was pressed/held: volume up
        }

        if (cardDetected)
        {
            return PREV_TRACK; //no button press: next track
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

    return NO_ACTION;
}

#ifdef UNIT_TEST
void UserInput_ClickEncoder::set_fake_user_request(UserInput::UserRequest_e fakeAction) // For unit testing: Injects fake requests
{
    switch (fakeAction)
        {
        case UserInput::NO_ACTION:
            buttonState = ClickEncoder::Open;
            encoderDiff = 0;
            set_card_detected(true);
            break;
        case UserInput::PLAY_PAUSE:
            set_card_detected(true);
            buttonState = ClickEncoder::Clicked;
            encoderDiff = 0;
            break;
        case UserInput::NEXT_TRACK:
            set_card_detected(true);
            buttonState = ClickEncoder::Open;
            encoderDiff = 1; 
            break;
        case UserInput::PREV_TRACK:
            set_card_detected(true);
            buttonState = ClickEncoder::Open;
            encoderDiff = -1; 
            break;
        case UserInput::INC_VOLUME:
            set_card_detected(true);
            buttonState = ClickEncoder::Held;
            encoderDiff = 1; 
            break;
        case UserInput::DEC_VOLUME:
            set_card_detected(true);
            buttonState = ClickEncoder::Held;
            encoderDiff = -1; 
            break;
        case UserInput::DelCard:
            set_card_detected(false);
            encoderDiff = 0;
            buttonState = ClickEncoder::DoubleClicked;
            break;
        case UserInput::Help:
            set_card_detected(false);
            encoderDiff = 0;
            buttonState = ClickEncoder::Held;
            break;
        case UserInput::Abort:
            set_card_detected(true);
            encoderDiff = 0;
            buttonState = ClickEncoder::Held;
            break;
        case UserInput::Error:
            state = inputPinsSet; //not ready anymore
            break;
        }
}
#endif

void UserInput_ClickEncoder::userinput_refresh()
{
    static int16_t oldEncPos = 0;

    //Get values from encoder
    encoderPosition += encoder->getValue();
    buttonState = encoder->getButton();

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
    state = inputPinsSet;
}

void UserInput_3Buttons::init()
{
    if (state != inputPinsSet)
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
    state = ready;
}

void UserInput_3Buttons::userinput_service_isr()
{
    if (state == ready)
    {
        prevButton->service();
        plpsButton->service();
        nextButton->service();
    }
}

UserInput::UserRequest_e UserInput_3Buttons::get_user_request()
{
    if (state != ready)
    {
        return Error;
    }
    else if (UserInput::userInputLocked)
    {
        return NO_ACTION;
    }

    //Get current button's states
    #ifndef UNIT_TEST
    userinput_refresh();
    #endif

    // --- PlayPauAbort button handler -----------------------------
    if (buttonStates.plpsButton == ClickEncoder::Clicked)
    {
        return PLAY_PAUSE;
    }
    else if (buttonStates.plpsButton == ClickEncoder::Held)
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
    else if (buttonStates.plpsButton == ClickEncoder::DoubleClicked)
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
    if ((buttonStates.nextButton == ClickEncoder::Clicked) && cardDetected)
    {
        return NEXT_TRACK;
    }

    if (buttonStates.nextButton == ClickEncoder::Held && cardDetected)
    {
        nextButton->set_long_press_active(true);
        if (nextButton->handle_repeat_long_pressed())
        {
            return INC_VOLUME;
        }
    }
    else
    {
        nextButton->set_long_press_active(false);
    }
    // --- Previous button handler ----------------------------------
    if ((buttonStates.prevButton == ClickEncoder::Clicked) && cardDetected)
    {
        return PREV_TRACK;
    }

    if (buttonStates.prevButton == ClickEncoder::Held && cardDetected)
    {
        prevButton->set_long_press_active(true);
        if (prevButton->handle_repeat_long_pressed())
        {
            return DEC_VOLUME;
        }
    }
    else
    {
        prevButton->set_long_press_active(false);
    }

    return NO_ACTION;
}

#ifdef UNIT_TEST
void UserInput_3Buttons::set_fake_user_request(UserInput::UserRequest_e fakeAction) // For unit testing: Injects fake requests
{
    switch (fakeAction)
        {
        case UserInput::NO_ACTION:
            set_card_detected(true);
            buttonStates.plpsButton = ClickEncoder::Open;
            buttonStates.nextButton = ClickEncoder::Open;
            buttonStates.prevButton = ClickEncoder::Open;
            break;
        case UserInput::PLAY_PAUSE:
            set_card_detected(true);
            buttonStates.plpsButton = ClickEncoder::Clicked;
            buttonStates.nextButton = ClickEncoder::Open;
            buttonStates.prevButton = ClickEncoder::Open;
            break;
        case UserInput::NEXT_TRACK:
            set_card_detected(true);
            buttonStates.plpsButton = ClickEncoder::Open;
            buttonStates.nextButton = ClickEncoder::Clicked;
            buttonStates.prevButton = ClickEncoder::Open;
            break;
        case UserInput::PREV_TRACK:
            set_card_detected(true);
            buttonStates.plpsButton = ClickEncoder::Open;
            buttonStates.nextButton = ClickEncoder::Open;
            buttonStates.prevButton = ClickEncoder::Clicked;
            break;
        case UserInput::INC_VOLUME:
            set_card_detected(true);
            buttonStates.plpsButton = ClickEncoder::Open;
            buttonStates.nextButton = ClickEncoder::Held;
            buttonStates.prevButton = ClickEncoder::Open;
            break;
        case UserInput::DEC_VOLUME:
            set_card_detected(true);
            buttonStates.plpsButton = ClickEncoder::Open;
            buttonStates.nextButton = ClickEncoder::Open;
            buttonStates.prevButton = ClickEncoder::Held;
            break;
        case UserInput::DelCard:
            set_card_detected(false);
            buttonStates.plpsButton = ClickEncoder::DoubleClicked;
            buttonStates.nextButton = ClickEncoder::Open;
            buttonStates.prevButton = ClickEncoder::Open;
            break;
        case UserInput::Help:
            set_card_detected(false);
            buttonStates.plpsButton = ClickEncoder::Held;
            buttonStates.nextButton = ClickEncoder::Open;
            buttonStates.prevButton = ClickEncoder::Open;
            break;
        case UserInput::Abort:
            set_card_detected(true);
            buttonStates.plpsButton = ClickEncoder::Held;
            buttonStates.nextButton = ClickEncoder::Open;
            buttonStates.prevButton = ClickEncoder::Open;
            break;
        case UserInput::Error:
            state = inputPinsSet; //not ready anymore
            break;
        }
}
#endif

void UserInput_3Buttons::userinput_refresh()
{
    buttonStates.plpsButton = plpsButton->getButton();
    buttonStates.nextButton = nextButton->getButton();
    buttonStates.prevButton = prevButton->getButton();
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
    bool result = false;
    if(longPressCount >= longPressRepeatInterval)
    {
        longPressCount = 0;
    }
    if (longPressCount == 0)
    {
        result = true;
    }
    return result;
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