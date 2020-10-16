#ifndef USERINPUT_IMPLEMENTATION_H
#define USERINPUT_IMPLEMENTATION_H

#include "../UserInput_interface/UserInput_interface.h"
#include "../ClickEncoder_interface/ClickEncoder_interface.h"
#include "../UserInput/ClickEncoder_implementation/ClickEncoder_supportsLongPress.h"
#include "Tonuino_config.h"

class UserInput_ClickEncoder : public UserInput
{
    //Interface implementation for clickEncoder
    //  NO_ACTION = 0,
    //  PLAY_PAUSE = Click
    //  NEXT_TRACK = Turn Right
    //  PREV_TRACK = Turn Left
    //  INC_VOLUME = Pressed/Held Turn Right
    //  DEC_VOLUME = Pressed/Held Turn Left
    //  ToggleLockInput = Doubleclick
    //  ERROR

public:
    // pinA, pinB, pinButton are the pins of the encoder that are connected to the uC.
    UserInput_ClickEncoder(ClickEncoder_interface *pEncoder) : m_pEncoder(pEncoder)
    {
        m_pEncoder->setAccelerationEnabled(true);
        m_pEncoder->setAccelerationEnabled(true);
    };

    ~UserInput_ClickEncoder()
    {
        delete m_pEncoder;
    };

public:
    void userinput_service_isr() override;
    UserRequest_e get_user_request() override;

private:
    void userinput_refresh() override;

private:
    ClickEncoder_interface *m_pEncoder;

    volatile int16_t encoderPosition{0};
    volatile int16_t encoderDiff{0};
    ClickEncoder_interface::eButtonState buttonState{ClickEncoder_interface::Open};
}; // UserInput_ClickEncoder

class UserInput_3Buttons : public UserInput
{
    //Interface implementation for 3 buttons
    //  NO_ACTION = 0,
    //  PLAY_PAUSE = plpsButton Click
    //  NEXT_TRACK = nextButton Click
    //  PREV_TRACK = prevButton Click
    //  INC_VOLUME = nextButton Held
    //  DEC_VOLUME = prevButton Held
    //  ToggleLockInput = plpsButton Doubleclick
    //  ERROR

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
                       const uint16_t &longPressDetectLevel)
    {
        m_pPlpsButton = new Encoder_longPressRepeat(pPlPsButton, ENC_LONGPRESSREPEATINTERVAL);
        m_pPlpsButton->setAccelerationEnabled(true);
        m_pPlpsButton->setDoubleClickEnabled(true);
        m_pNextButton = new Encoder_longPressRepeat(pNextButton, ENC_LONGPRESSREPEATINTERVAL);
        m_pNextButton->setAccelerationEnabled(true);
        m_pNextButton->setDoubleClickEnabled(true);
        m_pPrevButton = new Encoder_longPressRepeat(pPrevButton, ENC_LONGPRESSREPEATINTERVAL);
        m_pPrevButton->setAccelerationEnabled(true);
        m_pPrevButton->setDoubleClickEnabled(true);
    };
    ~UserInput_3Buttons() = default;

public:
    void userinput_service_isr(void) override;
    UserRequest_e get_user_request() override;

private:
    void userinput_refresh() override;

private:
    //OBJECTS
    Encoder_longPressRepeat *m_pPrevButton;
    Encoder_longPressRepeat *m_pPlpsButton;
    Encoder_longPressRepeat *m_pNextButton;
    ButtonStates buttonStates;
}; // UserInput_3Buttons


/*
class UserInput_Factory
{
    /*  
        This is the factory class, designed to create the right UserInput
        object based on input parameter, handing a pointer to therequested object back
        to the caller. 
    */
/*
public:
    /*  -------------------------------------------------------------------
    Depending on UserInput config, behavior will be as follows:
    -------------------------------------------------------------------
number 1,2,3... = Button/Encoder number
short press = SP
long press = LP
turn right = TR
turn left = TL
turn left pressed = PTL
turn right pressed = PTR
long press both buttons = LPB
place or remove Nfc Tag = TAG
     -------------------------------------------------------------------
userinput action  ENCODER_1  ENCODER_2  BUTTONS_2  BUTTONS_3  BUTTONS_4  BUTTONS_5   
PLAY_PAUSE,       SP            SP1         TAG     SP2         TAG         SP3
PP_LONGPRESS,     LP            SP1         LPB     LP2         LPB         LP3
NEXT_TRACK,       TR            TR1         SP2     SP3         SP4         SP5
PREV_TRACK,       TL            TL1         SP1     SP1         SP1         SP1
INC_VOLUME,       PTR           TR2         LP2     LP3         SP3         SP4
DEC_VOLUME,       PTL           TL2         LP1     LP1         SP2         SP2
    -------------------------------------------------------------------
*/
/*
    enum UserInputType_e
    {
        UNDEFINED = 0,
        ENCODER_1, // next
        TWO_ENCODERS,
        TWO_BUTTONS,
        THREE_BUTTONS,
        FOUR_BUTTONS,
        FIVE_BUTTONS
    };

public:
    static UserInput *getInstance(UserInputType_e typeKey);

}; // UserInput_Factory
*/
#endif // USERINPUT_IMPLEMENTATION_H