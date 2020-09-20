#ifndef USERINPUT_IMPLEMENTATION_H
#define USERINPUT_IMPLEMENTATION_H

#include <UserInput_interface.h>
#include "ClickEncoder.h"


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

    // Name friend classes so constructors can be called
    friend class UserInput_Factory;

public:
    UserInput_ClickEncoder();
    ~UserInput_ClickEncoder();

public:
    // pinA, pinB, pinSwitch are the pins of the encoder that are connected to the uC.
    void set_input_pins(uint8_t pinA, uint8_t pinB, uint8_t pinSwitch);
    void init();
    void userinput_service_isr();
    UserRequest_e get_user_request();
#ifdef UNIT_TEST
    void set_fake_user_request(UserRequest_e); // For unit testing: Injects fake requests
#endif

private:
    void userinput_refresh();

private:
    //OBJECTS
    // have to use pointer as object would have to be initialized
    // using its constructor while not all constructor's init variables are defined yet
    ClickEncoder *encoder;
    // INPUT PINS
    uint8_t pinA = 0;
    uint8_t pinB = 0;
    uint8_t pinSwitch = 0;
    // PARAMETERS AND DEFAULT VALUES
    InterfaceState_e state = uninitialized;
    bool switchActiveState = false;
    uint16_t doubleClickTime = 400;
    uint8_t encStepsPerNotch = 4;
    // Business logic variables
    volatile int16_t encoderPosition;
    volatile int16_t encoderDiff;
    ClickEncoder::Button buttonState;

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

    // Name friend classes so constructors can be called
    friend class UserInput_Factory;

private:
    struct ButtonStates
    {
        ClickEncoder::Button plpsButton = ClickEncoder::Open;
        ClickEncoder::Button nextButton = ClickEncoder::Open;
        ClickEncoder::Button prevButton = ClickEncoder::Open;
    };

    class DigitalButton_SupportsLongPress : public DigitalButton
    {
    public:
        //DigitalButton_SupportsLongPress(int8_t pinId, bool active, uint16_t longPressTime, uint16_t longPressRepeatInterval);
        DigitalButton_SupportsLongPress(int8_t pinId, bool active); //params to be set via set_parameters of parent class or uses default
        ~DigitalButton_SupportsLongPress();

    public:
        void service(void);
        void set_long_press_active(bool longPressActive);
        bool handle_repeat_long_pressed(void);

    private:
        uint16_t longPressTime;           //mSec
        uint16_t longPressRepeatInterval; //mSec
        volatile uint16_t longPressCount = 0;
        bool longPressActive = false;
        ClickEncoder::Button buttonState; //enum to hold buttonState
    };                                    //  DigitalButton_SupportsLongPress

public:
    /*
        First call set_input_pins to get arduino pins attached to the encoder
        Then call init to finally instantiate the encoder.
    */

    // pinPrev, pinePlayPause, pinNext are the pins of the buttons that are connected to the uC.
    void set_input_pins(uint8_t pinPlayPauseAbort, uint8_t pinPrev, uint8_t pinNext);
    void init();
    // Service routine to update button status (use 1ms task)
    void userinput_service_isr(void);
    UserRequest_e get_user_request();
#ifdef UNIT_TEST
    void set_fake_user_request(UserRequest_e); // For unit testing: Injects fake requests
#endif

private:
    void userinput_refresh();

private:
    //OBJECTS
    DigitalButton_SupportsLongPress *prevButton;
    DigitalButton_SupportsLongPress *plpsButton;
    DigitalButton_SupportsLongPress *nextButton;
    ButtonStates buttonStates;
    // INPUT PINS
    uint8_t pinPrev = 0;
    uint8_t pinPlayPauseAbort = 0;
    uint8_t pinNext = 0;
    // PARAMETERS
    InterfaceState_e state = uninitialized;
    bool switchActiveState;
    uint16_t doubleClickTime;         //mSec
    uint16_t longPressTime;           //mSec
    uint16_t longPressRepeatInterval; //mSec
};                                    // UserInput_3Buttons

class UserInput_Factory
{
    /*  This is the factory class, designed to create the right UserInput
        object based on input parameter, handing a pointer to therequested object back
        to the caller. 
        */

public:
    enum UserInputType_e
    {
        Undefined = 0,
        Encoder,
        ThreeButtons
    };

public:
    static UserInput *getInstance(UserInputType_e typeKey);

}; // UserInput_Factory

#endif // USERINPUT_IMPLEMENTATION_H