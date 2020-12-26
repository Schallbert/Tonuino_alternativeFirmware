#ifndef USERINPUT_IMPLEMENTATION_H
#define USERINPUT_IMPLEMENTATION_H

#include "../UserInput/UserInput_interface.h"
#include "ClickEncoder_Abstraction/ClickEncoder_interface.h"
#include "ClickEncoder_Abstraction/ClickEncoder_supportsLongPress.h"

class UserInput_ClickEncoder : public UserInput_interface
{
    //Interface implementation for clickEncoder
    //  NO_ACTION = 0,
    //  PLAY_PAUSE = Click
    //  NEXT_TRACK = Turn Right
    //  PREV_TRACK = Turn Left
    //  INC_VOLUME = Pressed/Held Turn Right
    //  DEC_VOLUME = Pressed/Held Turn Left
    //  ToggleLockInput = Doubleclick

public:
    // pinA, pinB, pinButton are the pins of the encoder that are connected to the uC.
    UserInput_ClickEncoder(ClickEncoder_interface *pEncoder) : m_pEncoder(pEncoder)
    {
        m_pEncoder->setAccelerationEnabled(false);
        m_pEncoder->setDoubleClickEnabled(true);
    };

    ~UserInput_ClickEncoder() = default;

public:
    void userinputServiceIsr() override;
    eUserRequest getUserRequest() override;

private:
    void userinputRefresh() override;

private:
    ClickEncoder_interface *m_pEncoder;

    volatile int16_t encoderPosition{0};
    volatile int16_t encoderDiff{0};
    ClickEncoder_interface::eButtonState buttonState{ClickEncoder_interface::Open};
}; // UserInput_ClickEncoder

class UserInput_3Buttons : public UserInput_interface
{
    //Interface implementation for 3 buttons
    //  NO_ACTION = 0,
    //  PLAY_PAUSE = plpsButton Click
    //  NEXT_TRACK = nextButton Click
    //  PREV_TRACK = prevButton Click
    //  INC_VOLUME = nextButton Held
    //  DEC_VOLUME = prevButton Held
    //  ToggleLockInput = plpsButton Doubleclick

private:
    struct ButtonStates
    {
        Encoder_longPressRepeat::eButtonState plpsButton{Encoder_longPressRepeat::Open};
        Encoder_longPressRepeat::eButtonState nextButton{Encoder_longPressRepeat::Open};
        Encoder_longPressRepeat::eButtonState prevButton{Encoder_longPressRepeat::Open};
    };

public:
    UserInput_3Buttons(ClickEncoder_interface *pPlPsButton,
                       ClickEncoder_interface *pNextButton,
                       ClickEncoder_interface *pPrevButton,
                       const uint16_t &longPressRepeatInterval)
    {
        m_pPlpsButton = new Encoder_longPressRepeat(pPlPsButton, longPressRepeatInterval);
        m_pPlpsButton->setAccelerationEnabled(false);
        m_pPlpsButton->setDoubleClickEnabled(true);
        m_pNextButton = new Encoder_longPressRepeat(pNextButton, longPressRepeatInterval);
        m_pNextButton->setAccelerationEnabled(false);
        m_pNextButton->setDoubleClickEnabled(true);
        m_pPrevButton = new Encoder_longPressRepeat(pPrevButton, longPressRepeatInterval);
        m_pPrevButton->setAccelerationEnabled(false);
        m_pPrevButton->setDoubleClickEnabled(true);
    };
    ~UserInput_3Buttons()
    {
        delete m_pPlpsButton;
        delete m_pNextButton;
        delete m_pPrevButton;
    };

public:
    void userinputServiceIsr(void) override;
    eUserRequest getUserRequest() override;

private:
    void userinputRefresh() override;

private:
    //OBJECTS
    Encoder_longPressRepeat *m_pPrevButton;
    Encoder_longPressRepeat *m_pPlpsButton;
    Encoder_longPressRepeat *m_pNextButton;
    ButtonStates buttonStates;
}; // UserInput_3Buttons

//    This is the factory class, designed to create the right UserInput
//    object based on input parameter, handing a pointer to the requested object back
//    to the caller.

// -------------------------------------------------------------------
// Depending on UserInput config, behavior will be as follows:
// -------------------------------------------------------------------
// number 1,2,3... = Button/Encoder number
// short press = SP
// long press = LP
// turn right = TR
// turn left = TL
// turn left pressed = PTL
// turn right pressed = PTR
// long press both buttons = LPB
// place or remove Nfc Tag = TAG
// -------------------------------------------------------------------
// userinput action  ONE_ENCODER  ENCODER_2  BUTTONS_2  BUTTONS_3  BUTTONS_4  BUTTONS_5
// PLAY_PAUSE,       SP            SP1         TAG     SP2         TAG         SP3
// PP_LONGPRESS,     LP            SP1         LPB     LP2         LPB         LP3
// NEXT_TRACK,       TR            TR1         SP2     SP3         SP4         SP5
// PREV_TRACK,       TL            TL1         SP1     SP1         SP1         SP1
// INC_VOLUME,       PTR           TR2         LP2     LP3         SP3         SP4
// DEC_VOLUME,       PTL           TL2         LP1     LP1         SP2         SP2
// -------------------------------------------------------------------

#endif // USERINPUT_IMPLEMENTATION_H