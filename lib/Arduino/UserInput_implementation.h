#ifndef USERINPUT_IMPLEMENTATION_H
#define USERINPUT_IMPLEMENTATION_H

#include "Mp3Prompt/Mp3Prompt_interface.h"
#include "MessageHandler_interface.h"

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
    //  ToggleLockInput = Doubleclick

public:
    // pinA, pinB, pinButton are the pins of the encoder that are connected to the uC.
    explicit UserInput_ClickEncoder(Mp3Prompt_interface &rPrompt,
                                    MessageHander_interface &rMessageHandler) : m_rPrompt(rPrompt),
                                                                                m_rMessageHandler(rMessageHandler)
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
    Mp3Prompt_interface &m_rPrompt;
    MessageHander_interface &m_rMessageHandler;

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
    //  ToggleLockInput = plpsButton Doubleclick

public:
    /*UserInput_3Buttons(Mp3Prompt_interface &rPrompt,
                       MessageHander_interface &rMessageHandler) : m_rPrompt(rPrompt),
                                                                   m_rMessageHandler(rMessageHandler)*/
    UserInput_3Buttons()
    {
        m_PlpsButton.setDoubleClickEnabled(true);
        m_NextButton.setLongPressRepeatEnabled(true);
        m_PrevButton.setLongPressRepeatEnabled(true);
    };
    ~UserInput_3Buttons() = default;

    void init() override;
    void userinputServiceIsr(void) override;
    Message::eMessageContent getUserRequest() override;

private:
    //Mp3Prompt_interface &m_rPrompt;
    //MessageHander_interface &m_rMessageHandler;

    Button m_PlpsButton{PINPLPS, USERINPUTACTIVE_STATE};
    Button m_NextButton{PINNEXT, USERINPUTACTIVE_STATE};
    Button m_PrevButton{PINPREV, USERINPUTACTIVE_STATE};

}; // UserInput_3Buttons

#endif // USERINPUT_IMPLEMENTATION_H