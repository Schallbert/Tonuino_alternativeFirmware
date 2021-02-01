#ifndef USERINPUT_FACTORY_H
#define USERINPUT_FACTORY_H

#include "Arduino_config.h"
#include "ClickEncoder/ClickEncoder_implementation.h"
#include "UserInput_implementation.h"

#include "Mp3Prompt/Mp3Prompt_interface.h"

class UserInput_interface;
class UserInput_factory
{
public:
    explicit UserInput_factory(Mp3Prompt_interface &rPrompt,
                               MessageHander_interface &rMessageHandler) : m_rPrompt(rPrompt),
                                                                           m_rMessageHandler(rMessageHandler){};
    ~UserInput_factory() = default;
    // TODO: delete move constructors of mostly all classes

    UserInput_interface *getInstance()
    {
        Message::eMessageContent message{Message::INPUTONLINE};
        if (m_pConcreteUserInput == nullptr)
        {
            message = Message::INPUTERROR;
        }
        m_rMessageHandler.printMessage(message);
        return m_pConcreteUserInput;
    }

private:
    Mp3Prompt_interface &m_rPrompt;
    MessageHander_interface &m_rMessageHandler;

#if (USERINPUT_VARIANT == three_buttons)
    ClickEncoder_implementation m_pinPlPs{ClickEncoder_implementation(PINPLPS, USERINPUTACTIVE_STATE)};
    ClickEncoder_implementation m_pinNext{ClickEncoder_implementation(PINNEXT, USERINPUTACTIVE_STATE)};
    ClickEncoder_implementation m_pinPrev{ClickEncoder_implementation(PINPREV, USERINPUTACTIVE_STATE)};
    UserInput_3Buttons instance{UserInput_3Buttons(
        m_pinPlPs,
        m_pinNext,
        m_pinPrev,
        m_rPrompt,
        m_rMessageHandler)};
    UserInput_interface *m_pConcreteUserInput{&instance};

#elif (USERINPUT_VARIANT == one_encoder)
    ClickEncoder_implementation instance{ClickEncoder_implementation(ENCA,
                                                                     ENCB,
                                                                     ENCSW,
                                                                     rPrompt,
                                                                     rMessageHandler,
                                                                     ENC_STEPSPERNOTCH,
                                                                     USERINPUTACTIVE_STATE)};
    UserInput_interface *m_pConcreteUserInput{&instance};
#else
UserInput_interface *m_pConcreteUserInput{nullptr};
#endif
};

#endif // USERINPUT_FACTORY_h
