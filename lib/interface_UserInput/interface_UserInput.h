#ifndef USERINPUT_H
#define USERINPUT_H

#ifndef ARDUINO_H
#include "Arduino.h"
#endif

#include "ClickEncoder.h"

class UserInput
{
    //Interface class of UserInput implementation
    /*  UserInput gets the user's requests for device control,
        be it via buttons or a click encoder, and relays the requests
        to the main program.
        This interface is set up to make the main program independent of
        the physical implementation of the user input.
    */
public:
    enum UserRequest_e
    {
        NoAction,
        PlayPause,
        NextTrack,
        PrevTrack,
        IncVolume,
        DecVolume,
        DelCard,
        Help,
        Abort,
        Error
    };

protected:
    //UserInput();
    //~UserInput();
    bool userInputLocked = false;
    bool cardDetected = false;

    enum InterfaceState_e
    {
        uninitialized = 0,
        inputPinsSet = 1,
        ready = 2
    };

public:
    // methods
    virtual void set_input_pins(uint8_t, uint8_t, uint8_t) = 0; // Must have same signature as derived methods
    virtual void init(void) = 0;                                // Initializes userInput object
    virtual UserRequest_e get_user_request(void) = 0;           // returns user's request to main program
    virtual void userinput_service_isr(void) = 0;               // recurring task to poll UserInput's connected hardware
    void set_card_detected(bool cardDetected) {this->cardDetected = cardDetected;} // input: card status from main
#ifdef UNIT_TEST
    virtual void set_fake_user_request(UserRequest_e) = 0; // For unit testing: Injects fake requests
#endif

private:
    virtual void userinput_refresh(void) = 0; // refreshes button status to UserInput class

}; // UserInput

class UserInput_ClickEncoder : public UserInput
{
    //Interface implementation for clickEncoder
    //  NoAction = 0,
    //  PlayPause = Click
    //  NextTrack = Turn Right with card
    //  PrevTrack = Turn Left with card
    //  IncVolume = Pressed/Held Turn Right
    //  DecVolume = Pressed/Held Turn Left
    //  DelCard =   Doubleclick without card
    //  ToggleLockInput = Doubleclick with card
    //  Help = Held without card

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
    //  NoAction = 0,
    //  PlayPause = plpsButton Click
    //  NextTrack = nextButton Click with card
    //  PrevTrack = prevButton Click with card
    //  IncVolume = nextButton Held
    //  DecVolume = prevButton Held
    //  DelCard =   plpsButton Doubleclick without card
    //  toggleLockInput = plpsButton Doubleclick with card
    //  Help = plpsButton Held without card

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

    static UserInput *getInstance(UserInputType_e typeKey);

};     // UserInput_Factory
#endif // USERINPUT_H