#ifndef USERINPUT_IMPLEMENTATION_H
#define USERINPUT_IMPLEMENTATION_H

#include "MessageHandler_interface.h"
#include "Mp3Prompt/Mp3Prompt_interface.h"

#include "UserInput/UserInput_interface.h"

#include "ClickEncoder.h"

#include "Arduino_config.h"
#include "Tonuino_config.h"

class UserInput_ClickEncoder : public UserInput_interface
{
    //Interface implementation for clickEncoder
    //  NO_ACTION = 0,
    //  PLAY_PAUSE = Click
    //  NEXT_TRACK = Turn Right
    //  PREV_TRACK = Turn Left
    //  INC_VOLUME = Pressed/Held Turn Right
    //  DEC_VOLUME = Pressed/Held Turn Left
    //  ToggleLockInput = plpsButton Doubleclick when Tag present
    //  Delete Menu == plPsButton DoubleClick without Tag
    //  Help == PlPsButton Held

public:
    // pinA, pinB, pinButton are the pins of the encoder that are connected to the uC.
    explicit UserInput_ClickEncoder()
    {
        m_Encoder.setDoubleClickEnabled(true);
    }

    ~UserInput_ClickEncoder() = default;
    UserInput_ClickEncoder(const UserInput_ClickEncoder &cpySrc) = delete;

    void init() override;
    void userinputServiceIsr() override;
    Message::eMessageContent getUserRequest() override;

private:
    void userinputRefresh(); // refreshes button status to UserInput class

private:
    ClickEncoder m_Encoder{ENCA, ENCB, ENCSW, ENC_STEPSPERNOTCH, USERINPUTACTIVE_STATE};

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
    //  ToggleLockInput = plpsButton Doubleclick when Tag present
    //  Delete Menu == plPsButton DoubleClick withoug Tag
    //  Help == PlPsButton Held

public:
    UserInput_3Buttons()
    {
        m_PlpsButton.setDoubleClickEnabled(true);
        m_NextButton.setLongPressRepeatEnabled(true);
        m_PrevButton.setLongPressRepeatEnabled(true);
    }
    ~UserInput_3Buttons() = default;

    void init() override;
    void userinputServiceIsr(void) override;
    Message::eMessageContent getUserRequest() override;

private:
    Button m_PlpsButton{PINPLPS, USERINPUTACTIVE_STATE};
    Button m_NextButton{PINNEXT, USERINPUTACTIVE_STATE};
    Button m_PrevButton{PINPREV, USERINPUTACTIVE_STATE};

}; // UserInput_3Buttons

class UserInput_2Buttons : public UserInput_interface
{
    //Interface implementation for 3 buttons
    //  NO_ACTION = 0,
    //  PLAY_PAUSE = NONE
    //  NEXT_TRACK = nextButton Click
    //  PREV_TRACK = prevButton Click
    //  INC_VOLUME = nextButton Held
    //  DEC_VOLUME = prevButton Held
    //  ToggleLockInput = prevButton Doubleclick when Tag present
    //  Delete Menu == prevButton DoubleClick without Tag
    //  Help == NONE

public:
    UserInput_2Buttons()
    {
        m_PrevButton.setDoubleClickEnabled(true);
        m_PrevButton.setLongPressRepeatEnabled(true);
        m_NextButton.setLongPressRepeatEnabled(true);
    }
    ~UserInput_2Buttons() = default;

    void init() override;
    void userinputServiceIsr(void) override;
    Message::eMessageContent getUserRequest() override;

private:
    Button m_NextButton{PINNEXT, USERINPUTACTIVE_STATE};
    Button m_PrevButton{PINPREV, USERINPUTACTIVE_STATE};
}; // UserINput_2Buttons

#endif // USERINPUT_IMPLEMENTATION_H