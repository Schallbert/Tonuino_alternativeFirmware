#ifndef USERINPUT_IMPLEMENTATION_H
#define USERINPUT_IMPLEMENTATION_H

#include "Mp3Prompt/Mp3Prompt_interface.h"
#include "MessageHandler_interface.h"

#include "../UserInput/UserInput_interface.h"
#include "ClickEncoder_Abstraction/ClickEncoder_interface.h"

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
    explicit UserInput_ClickEncoder(ClickEncoder_interface &rEncoder,
                                    Mp3Prompt_interface &rPrompt,
                                    MessageHander_interface &rMessageHandler) : m_rEncoder(rEncoder),
                                                                                m_rPrompt(rPrompt),
                                                                                m_rMessageHandler(rMessageHandler)
    {
        m_rEncoder.setDoubleClickEnabled(true);
    };
    ~UserInput_ClickEncoder() = default;

    void userinputServiceIsr() override;
    Message::eMessageContent getUserRequest() override;

private:
    void userinputRefresh(); // refreshes button status to UserInput class

private:
    ClickEncoder_interface &m_rEncoder;
    Mp3Prompt_interface &m_rPrompt;
    MessageHander_interface &m_rMessageHandler;

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
        ClickEncoder_interface::eButtonState plpsButton{ClickEncoder_interface::Open};
        ClickEncoder_interface::eButtonState nextButton{ClickEncoder_interface::Open};
        ClickEncoder_interface::eButtonState prevButton{ClickEncoder_interface::Open};
    };

public:
    UserInput_3Buttons(ClickEncoder_interface &rPlPsButton,
                       ClickEncoder_interface &rNextButton,
                       ClickEncoder_interface &rPrevButton,
                       Mp3Prompt_interface &rPrompt,
                       MessageHander_interface &rMessageHandler) : m_PlpsButton(rPlPsButton),
                                                                   m_NextButton(rNextButton),
                                                                   m_PrevButton(rPrevButton),
                                                                   m_rPrompt(rPrompt),
                                                                   m_rMessageHandler(rMessageHandler)

    {
        m_PlpsButton.setDoubleClickEnabled(true);
        m_NextButton.setLongPressRepeatEnabled(true);
        m_PrevButton.setLongPressRepeatEnabled(true);
    };
    ~UserInput_3Buttons() = default;

    void userinputServiceIsr(void) override;
    Message::eMessageContent getUserRequest() override;

private:
    void userinputRefresh();

private:
    ClickEncoder_interface &m_PlpsButton;
    ClickEncoder_interface &m_NextButton;
    ClickEncoder_interface &m_PrevButton;
    Mp3Prompt_interface &m_rPrompt;
    MessageHander_interface &m_rMessageHandler;

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